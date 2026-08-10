#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")"
docker build -t tofuctf-warm-tofu .
docker rm -f tofuctf-warm-tofu >/dev/null 2>&1 || true
printf 'Paste the launch token copied from TofuCTF: '
IFS= read -r TOFUCTF_LAUNCH_TOKEN
[ -n "$TOFUCTF_LAUNCH_TOKEN" ] || { echo 'A launch token is required.' >&2; exit 1; }
docker run --rm --name tofuctf-warm-tofu \
  --cap-drop=ALL \
  --security-opt=no-new-privileges \
  -e TOFUCTF_LAUNCH_TOKEN="$TOFUCTF_LAUNCH_TOKEN" \
  -p 127.0.0.1:31337:31337 \
  tofuctf-warm-tofu
