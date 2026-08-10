#!/usr/bin/env sh
set -eu

# Fetch this account's signed flag inside the container. The launch token is
# short-lived and is not the flag itself.
: "${TOFUCTF_LAUNCH_TOKEN:?Paste a launch token from the TofuCTF site when running run.sh}"
umask 077
curl -fsS --retry 2 --data-binary "$TOFUCTF_LAUNCH_TOKEN" \
  https://tofuctf-auth.tofu-lab.workers.dev/api/instance-flag > /flag
chown root:ctf /flag
chmod 440 /flag
unset TOFUCTF_LAUNCH_TOKEN

exec gosu ctf socat TCP-LISTEN:31337,reuseaddr,fork EXEC:/home/ctf/warm-tofu,stderr
