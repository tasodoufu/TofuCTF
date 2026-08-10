#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")"
umask 077
printf 'TofuCTF{%s}\n' "$(openssl rand -hex 16)" > .flag
chmod 444 .flag
docker build -t tofuctf-warm-tofu .
docker rm -f tofuctf-warm-tofu >/dev/null 2>&1 || true
docker run --rm --name tofuctf-warm-tofu -p 31337:31337 -v "$(pwd)/.flag:/flag:ro" tofuctf-warm-tofu
