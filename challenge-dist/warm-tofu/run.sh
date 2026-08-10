#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")"
docker build -t tofuctf-warm-tofu .
docker rm -f tofuctf-warm-tofu >/dev/null 2>&1 || true
docker run --rm --name tofuctf-warm-tofu -p 31337:31337 tofuctf-warm-tofu
