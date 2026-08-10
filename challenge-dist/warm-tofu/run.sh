#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")"
docker build -t tofuctf-warm-tofu .
docker rm -f tofuctf-warm-tofu >/dev/null 2>&1 || true
docker run --rm --name tofuctf-warm-tofu \
  --cap-drop=ALL \
  --security-opt=no-new-privileges \
  -p 127.0.0.1:31337:31337 \
  tofuctf-warm-tofu
