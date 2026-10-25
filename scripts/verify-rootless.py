#!/usr/bin/env python3
"""Author-only local integration gate; no publishing and no system Docker fallback."""
import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import signal
import socket
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parent.parent
UID = os.getuid()
HOST = f"unix:///run/user/{UID}/tofuctf-docker/docker.sock"
STATE = Path.home() / ".local/state/tofuctf-rootless"
ENV = dict(os.environ, DOCKER_HOST=HOST, DOCKER_CONFIG=str(Path.home()/'.docker'))
for key in ('DOCKER_CONTEXT', 'DOCKER_TLS_VERIFY', 'DOCKER_CERT_PATH', 'BUILDX_BUILDER', 'BUILDKIT_HOST'):
    ENV.pop(key, None)


def run(args, **kwargs):
    return subprocess.run(args, env=ENV, check=True, timeout=kwargs.pop('timeout', 60), **kwargs)


def docker(*args, **kwargs):
    return run(['docker', *args], **kwargs)


def out(*args):
    return docker(*args, capture_output=True, text=True).stdout.strip()


def start_distribution(script, directory, env):
    # Stop the shell AND its Docker CLI children before owned-resource cleanup.
    # Killing only the shell can leave a child creating resources after finally.
    proc = subprocess.Popen(['sh', str(script)], cwd=directory, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            start_new_session=True)
    try:
        stdout, stderr = proc.communicate(timeout=600)
    except BaseException:
        try:
            os.killpg(proc.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        proc.communicate()
        raise
    if proc.returncode:
        raise subprocess.CalledProcessError(proc.returncode, ['sh', str(script)],
                                            output=stdout, stderr=stderr)


def inspect_optional(name):
    ids = out('ps', '-aq', '--filter', f'name=^/{name}$').splitlines()
    return json.loads(out('inspect', ids[0]))[0] if ids else None


def wait_ready(name, port, timeout):
    deadline = time.monotonic() + timeout
    last_error = 'not attempted'
    while time.monotonic() < deadline:
        config = inspect_optional(name)
        if not config or not config['State']['Running']:
            raise RuntimeError('container exited before TCP readiness')
        try:
            with socket.create_connection(('127.0.0.1', port), timeout=0.5):
                return
        except OSError as error:
            last_error = type(error).__name__
            time.sleep(0.15)
    raise RuntimeError(f'TCP readiness deadline exceeded; last exception={last_error}')


def passive_startup_diagnostic(port):
    # Read-only, bounded fresh connection: no challenge input or author payload.
    # Useful when socat is healthy but its application immediately fails to load.
    try:
        with socket.create_connection(('127.0.0.1', port), timeout=2) as sock:
            sock.settimeout(2)
            data = sock.recv(4096)
        loader_error = b'GLIBC_' in data and b'not found' in data
        print(f'PASSIVE_DIAGNOSTIC: response_bytes={len(data)}; '
              f'dynamic_loader_missing_glibc={loader_error}; raw output suppressed',
              file=sys.stderr, flush=True)
    except OSError as error:
        print(f'PASSIVE_DIAGNOSTIC: exception_type={type(error).__name__}',
              file=sys.stderr, flush=True)


def safe_author_diagnostic(result):
    # Exception messages and test output may contain flags: only emit categories/locations.
    combined = result.stderr + result.stdout
    kinds = re.findall(r'^([A-Za-z][A-Za-z0-9_]*(?:Error|Exception)):', combined, re.M)
    lines = re.findall(r'File "[^"\n]*.py", line ([0-9]+)', combined)
    print('AUTHOR_DIAGNOSTIC: exception_types=' + ','.join(kinds or ['unclassified'])
          + '; traceback_lines=' + ','.join(lines) + '; raw output suppressed',
          file=sys.stderr, flush=True)
    if 'exploit failed; received' in combined:
        print('AUTHOR_DIAGNOSTIC: existing test reported an application-output mismatch',
              file=sys.stderr, flush=True)
    if 'service closed before prompt' in combined:
        print('AUTHOR_DIAGNOSTIC: service EOF before expected application prompt',
              file=sys.stderr, flush=True)


def safe_runtime_diagnostic(name):
    try:
        config = inspect_optional(name)
        if not config:
            print('RUNTIME_DIAGNOSTIC: owned container absent', file=sys.stderr, flush=True)
            return
        state = config['State']
        print('RUNTIME_DIAGNOSTIC: ' + json.dumps({key: state.get(key) for key in
              ('Status', 'Running', 'ExitCode', 'OOMKilled', 'Restarting')})
              + f'; restart_count={config.get("RestartCount", 0)}', file=sys.stderr, flush=True)
        result = docker('logs', '--tail', '30', name, capture_output=True, text=True)
        logs = result.stdout + result.stderr
        categories = [label for label, pattern in (
            ('exec-format', 'Exec format error'), ('permission', 'Permission denied'),
            ('missing-file', 'No such file or directory'), ('connection-refused', 'Connection refused'),
            ('segmentation-fault', 'Segmentation fault'), ('bind-in-use', 'Address already in use'),
            ('dynamic-loader', 'GLIBC_')) if pattern in logs]
        print('RUNTIME_DIAGNOSTIC: log_categories=' + ','.join(categories or ['none-known'])
              + '; raw logs suppressed', file=sys.stderr, flush=True)
    except Exception as error:
        print(f'RUNTIME_DIAGNOSTIC: unavailable ({type(error).__name__})', file=sys.stderr, flush=True)


def cleanup_owned(name, volume, image, image_existed, owned_id):
    named = inspect_optional(name)
    if named:
        if owned_id and named['Id'] != owned_id:
            raise RuntimeError('owned container ID changed; refusing cleanup of replacement')
        docker('rm', '-f', named['Id'], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    # An interrupted run.sh may leave its anonymous --rm initializer. Only exact
    # owned-volume + exact image + auto-remove matches are eligible for removal.
    for cid in out('ps', '-aq', '--filter', f'volume={volume}').splitlines():
        config = json.loads(out('inspect', cid))[0]
        exact_mount = any(m.get('Type') == 'volume' and m.get('Name') == volume
                          for m in config['Mounts'])
        if not (exact_mount and config['Config']['Image'] == image
                and config['HostConfig']['AutoRemove']):
            raise RuntimeError('owned volume has an unexpected user; refusing cleanup')
        docker('rm', '-f', config['Id'], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if volume in out('volume', 'ls', '-q').splitlines():
        docker('volume', 'rm', volume, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if not image_existed and out('image', 'ls', '-q', image):
        docker('image', 'rm', image, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if inspect_optional(name) or volume in out('volume', 'ls', '-q').splitlines():
        raise RuntimeError('owned resources remained after cleanup')
    print('CLEANUP_OK: owned containers/flag-volume removed; unrelated resources untouched', flush=True)


def main():
    if not __debug__:
        raise RuntimeError('optimized Python disables safety assertions; refusing to run')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('slug', help='challenge-dist/<slug> and existing author/exploit-<slug>.py')
    parser.add_argument('--ready-timeout', type=float, default=20, help='TCP readiness deadline in seconds')
    parser.add_argument('--port', type=int, default=0, help='0 selects an unused loopback port')
    parser.add_argument('--fail-after-start', action='store_true', help='test failure cleanup')
    args = parser.parse_args()
    if not re.fullmatch(r'[a-z0-9]+(?:-[a-z0-9]+)*', args.slug):
        raise RuntimeError('invalid challenge slug')
    if not 0 < args.ready_timeout <= 120:
        raise RuntimeError('ready timeout must be in (0, 120] seconds')
    slug = args.slug
    directory = ROOT / 'challenge-dist' / slug
    exploit = ROOT / 'author' / f'exploit-{slug}.py'
    script = directory / 'run.sh'
    if not script.is_file() or not exploit.is_file():
        raise RuntimeError('challenge run.sh and author-only exploit are required')
    if args.port == 31337 or (args.port and not 1024 <= args.port <= 65535):
        raise RuntimeError('verification requires an unprivileged port other than 31337')
    STATE.mkdir(mode=0o700, parents=True, exist_ok=True)
    lock = (STATE / 'verify.lock').open('w')
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        raise RuntimeError('another dedicated verification is running') from None
    run(['systemctl', '--user', 'start', 'tofuctf-docker.service'])
    info = json.loads(out('info', '--format', '{{json .}}'))
    expected_root = str(Path.home() / '.local/share/tofuctf-docker')
    if not ('name=rootless' in info['SecurityOptions'] and info['CgroupDriver'] == 'systemd'
            and info['CgroupVersion'] == '2' and info['DockerRootDir'] == expected_root):
        raise RuntimeError('not the dedicated rootless cgroup-v2 daemon; refusing fallback')
    print('ROOTLESS_ENDPOINT_OK', flush=True)
    # Never let run.sh replace existing resources with its fixed names.
    name, volume = f'tofuctf-{slug}', f'tofuctf-{slug}-flag'
    if name in out('ps', '-a', '--format', '{{.Names}}').splitlines():
        raise RuntimeError('owned container name already exists; refusing run.sh start')
    if volume in out('volume', 'ls', '-q').splitlines():
        raise RuntimeError('owned volume name already exists; refusing run.sh start')
    image = f'tofuctf/{slug}:local'
    image_existed = bool(out('image', 'ls', '-q', image))
    with socket.socket() as probe:
        probe.bind(('127.0.0.1', args.port))
        port = probe.getsockname()[1]
    if port == 31337:
        raise RuntimeError('ephemeral allocator selected reserved port; retry')
    child_env = dict(ENV, PORT=str(port))
    # All owned-resource creation happens within the cleanup scope below.
    def interrupted(signum, frame):
        raise RuntimeError(f'interrupted by signal {signum}')
    for sig in (signal.SIGTERM, signal.SIGHUP):
        signal.signal(sig, interrupted)
    started = False
    owned_id = None
    phase = 'startup'
    try:
        started = True
        start_distribution(script, directory, child_env)
        config = json.loads(out('inspect', name))[0]
        owned_id = config['Id']
        phase = 'runtime-limits'
        hc = config['HostConfig']
        assert hc['ReadonlyRootfs'] and 'ALL' in hc['CapDrop']
        assert 'no-new-privileges' in hc['SecurityOpt']
        assert hc['Memory'] == 134217728 and hc['MemorySwap'] == 134217728
        assert hc['NanoCpus'] == 500000000 and hc['PidsLimit'] == 64
        bindings = config['NetworkSettings']['Ports']['31337/tcp']
        assert bindings == [{'HostIp':'127.0.0.1','HostPort':str(port)}]
        limits = out('exec', name, 'sh', '-c',
                     'cat /sys/fs/cgroup/cpu.max /sys/fs/cgroup/memory.max '
                     '/sys/fs/cgroup/memory.swap.max /sys/fs/cgroup/pids.max')
        assert limits.splitlines() == ['50000 100000','134217728','0','64'], limits
        sandbox = out('exec', name, 'sh', '-c',
                      'grep -E "CapEff|NoNewPrivs|Seccomp:" /proc/1/status; ulimit -n')
        assert '0000000000000000' in sandbox
        assert re.search(r'NoNewPrivs:[ ]*1|NoNewPrivs:\s+1', sandbox)
        assert re.search(r'Seccomp:\s+2', sandbox)
        assert sandbox.splitlines()[-1] == '128'
        print(f'RUNTIME_LIMITS_OK cpu=0.5 memory=128MiB swap=0 pids=64 nofile=128; localhost:{port}', flush=True)
        phase = 'tcp-readiness'
        wait_ready(name, port, args.ready_timeout)
        print('TCP_READY_OK: loopback connection accepted', flush=True)
        if args.fail_after_start:
            phase = 'intentional-failure'
            raise RuntimeError('intentional failure-cleanup test')
        # Never emit the recovered flag, even if the author exploit fails.
        phase = 'author-test'
        result = subprocess.run(['python3', str(exploit), '127.0.0.1', str(port)],
                                env=ENV, capture_output=True, text=True, timeout=45)
        if result.returncode:
            safe_author_diagnostic(result)
            passive_startup_diagnostic(port)
            raise RuntimeError(f'existing author test exited {result.returncode}; no test code changed')
        phase = 'metadata-check'
        flags = re.findall(r'TofuCTF[{][0-9a-f]{32}[}]', result.stdout)
        entries = json.loads((ROOT/'challenges.json').read_text())
        entry = next(x for x in entries if x['id'] == slug)
        assert flags and hashlib.sha256(flags[-1].encode()).hexdigest() == entry['flagHash']
        print('AUTHOR_EXPLOIT_OK; metadata flagHash matched; flag suppressed', flush=True)
    except BaseException:
        print(f'FAILURE_PHASE: {phase}', file=sys.stderr, flush=True)
        if started:
            safe_runtime_diagnostic(name)
        raise
    finally:
        if started:
            cleanup_owned(name, volume, image, image_existed, owned_id)
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except subprocess.TimeoutExpired:
        print('VERIFY_FAILED: command timeout; captured output suppressed', file=sys.stderr)
        sys.exit(1)
    except subprocess.CalledProcessError as error:
        # Captured output may contain a flag or a credential; do not forward it.
        print(f'VERIFY_FAILED: command exited {error.returncode}; captured output suppressed', file=sys.stderr)
        sys.exit(1)
    except Exception as error:
        # Only verifier-authored RuntimeErrors are safe; other messages may hold output.
        detail = str(error) if isinstance(error, RuntimeError) else type(error).__name__
        print(f'VERIFY_FAILED: {detail}', file=sys.stderr)
        sys.exit(1)
