#!/bin/sh
set -eu
docker rm -f tofuctf-warm-tofu >/dev/null 2>&1 || true
docker volume rm tofuctf-warm-tofu-flag >/dev/null 2>&1 || true
echo "Warm Tofu stopped."
