#!/bin/sh
set -eu
docker rm -f tofuctf-cool-tofu >/dev/null 2>&1 || true
docker volume rm tofuctf-cool-tofu-flag >/dev/null 2>&1 || true
echo "Cool Tofu stopped."
