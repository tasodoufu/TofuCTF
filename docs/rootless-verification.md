# Author-only rootless verification

## Dedicated per-user endpoint

- User service: `systemctl --user status tofuctf-docker.service` (enabled).
- Socket: `unix:///run/user/<uid>/tofuctf-docker/docker.sock` (the gate uses the current UID).
- Data: `~/.local/share/tofuctf-docker`.
- Daemon config: `~/.config/tofuctf-docker/daemon.json`.
- Unit: `~/.config/systemd/user/tofuctf-docker.service`.
- Never fall back to system Docker or another user's daemon. Do not change groups,
  socket permissions, existing Docker services, or the live port 31337.

The gate pins the socket itself, clears Docker context/TLS overrides, starts only
this user service, and requires rootless + systemd + cgroup v2 + the exact data
root. An installed Docker CLI alone is not evidence of a usable safe endpoint.

## Generic daily entry point

From this repository:

    python3 -B scripts/verify-rootless.py <slug>

For example, the 2026-10-25 challenge is oob-oolong. The slug must have an existing
challenge-dist/<slug>/run.sh and an existing author/exploit-<slug>.py. No test code,
challenge binary, source, distribution archive, metadata, or publication is changed
by this gate. It does invoke the existing run.sh (including its Docker build) and
existing author test. Future daily jobs should use the same entry point with their
new slug, never copy a one-off Docker command or weaken the resource checks.

Optional: --port <unused-loopback-port> (not 31337), --ready-timeout 20 (maximum
120 seconds). The default uses a dynamically selected loopback port. The brief
release/bind race fails closed rather than killing an existing listener.
Do not use Python -O or PYTHONOPTIMIZE; the gate refuses disabled assertions.

## Isolation and cleanup contract

- Nonblocking flock: ~/.local/state/tofuctf-rootless/verify.lock. Concurrent gate
  invocations fail before container creation. All manual use of this dedicated
  daemon must honor this lock too; it is not a security boundary against a user
  who deliberately bypasses it.
- Before run.sh, reject an existing exact tofuctf-<slug> container or
  tofuctf-<slug>-flag volume. Existing unrelated containers/volumes are allowed.
- Check actual runtime cgroups: CPU 0.5, memory 128 MiB, swap 0, pids 64,
  nofile 128, read-only root, dropped capabilities, no-new-privileges, seccomp,
  and loopback-only port mapping. No fallback or relaxed success criteria.
- Wait for TCP readiness before invoking the unchanged author test. TCP acceptance
  proves transport only, not a healthy application or a successful author test.
- Finally remove only the owned exact container/observed ID and exact flag volume.
  An interrupted run.sh anonymous --rm initializer is eligible only if it mounts
  the exact previously absent owned volume and uses the expected image.
- Preserve pre-existing images; remove only a newly introduced challenge image tag.
  Build caches/base images may remain. No prune, daemon-wide container deletion,
  unrelated-volume deletion, or deployment-helper calls.
- Ordinary failures, timeouts, SIGINT/SIGTERM/SIGHUP reach cleanup. SIGKILL, host
  failure, or loss of daemon access cannot guarantee cleanup. If the next run
  refuses a collision, inspect that exact resource privately; never prune or
  blindly retry run.sh, whose fixed-name startup is destructive.

## Result interpretation

Exit 0 requires ROOTLESS_ENDPOINT_OK, RUNTIME_LIMITS_OK, TCP_READY_OK,
AUTHOR_EXPLOIT_OK with the recovered flag matching challenges.json flagHash, and
CLEANUP_OK. Never report full integration success for transport-only success.
The flag and raw author/build/container outputs are suppressed. Failure output
identifies the phase, safe exception types/traceback line numbers, container
running/exit/OOM/restart state, and allowlisted operational log categories.
Do not paste raw author output, /flag, /secrets/flag, or Docker inspect environment
values into chat/logs.

Failure cleanup smoke test (expected exit 1, no author test executed):

    python3 -B scripts/verify-rootless.py <slug> --fail-after-start

Require the intentional-failure message AND CLEANUP_OK; exit 1 by itself is not
proof of successful cleanup. A small test-only unrelated stopped container and
volume can be used to confirm preservation, then removed by their recorded exact
ID/name. Never use production objects as disposable test fixtures.

## Verified 2026-10-25

- Rootless endpoint, runtime resource/security checks, loopback TCP readiness,
  unchanged author test, metadata flagHash match, and owned-resource cleanup:
  PASS for oob-oolong; the full gate exited 0.
- The first integration test exposed an existing packaging mismatch: the release
  ELF requires GLIBC_2.38, but Debian bookworm supplies glibc 2.36. Passive startup
  diagnostics confirmed the missing version without printing raw responses.
  The Dockerfile now uses the maintained official Debian trixie-slim image
  (runtime glibc 2.41), and the distribution archive was rebuilt from the exact
  challenge-dist/oob-oolong directory. Binary, source, author test, metadata,
  run.sh, and flagHash were not changed. This packaging fix was verified locally;
  local success alone does not establish that a published archive was replaced.
- Intentional failure after readiness: PASS cleanup. An unrelated test container
  and volume survived with the same ID/name. Exact-name container/volume collision
  refusals, concurrent flock refusal, and optimized-Python refusal: PASS.
- Existing system docker/docker.socket/containerd and port 31337 were read-only
  observed, not changed. The dedicated user service stays active.

## Daily automation verification contract

Run python3 -B scripts/verify-rootless.py <new-slug> from the tofu-ctf repository
as the dedicated daemon owner for dynamic verification, using only its pinned rootless
Docker endpoint. Do not use system/other-user Docker or port 31337. Require exit
0 and all success markers including cleanup; if unavailable or failing, record
that blocker and do not commit or publish a new daily problem. Do not modify the
existing author test to force a pass. Keep all existing publication, count/date,
metadata, and authentication rules unchanged.
