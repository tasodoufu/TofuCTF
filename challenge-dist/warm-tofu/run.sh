#!/bin/sh
set -eu

image=tofuctf/warm-tofu:local
container=tofuctf-warm-tofu
volume=tofuctf-warm-tofu-flag
port="${PORT:-31337}"
slug=warm-tofu

command -v docker >/dev/null 2>&1 || { echo "Docker is required." >&2; exit 1; }

case "${1:-start}" in
  start) ;;
  stop)
    docker rm -f "$container" >/dev/null 2>&1 || true
    docker volume rm "$volume" >/dev/null 2>&1 || true
    echo "Warm Tofu stopped."
    exit 0
    ;;
  *) echo "Usage: $0 [start|stop]" >&2; exit 2 ;;
esac

command -v sha256sum >/dev/null 2>&1 || { echo "sha256sum is required." >&2; exit 1; }

binary_hash="$(sha256sum "./$slug" | awk '{print $1}')"
flag_hex="$(printf '%s' "tofuctf-local-v1:$slug:$binary_hash" | sha256sum | cut -c1-32)"
flag="TofuCTF{$flag_hex}"
unset binary_hash flag_hex

docker build -t "$image" .
docker rm -f "$container" >/dev/null 2>&1 || true
docker volume rm "$volume" >/dev/null 2>&1 || true
docker volume create "$volume" >/dev/null
printf '%s\n' "$flag" | docker run --rm -i --user 0 --entrypoint sh \
  --mount "type=volume,source=$volume,target=/secrets" "$image" \
  -c 'umask 077; cat > /secrets/flag; chown 1000:1000 /secrets/flag; chmod 0400 /secrets/flag'
unset flag

docker run -d --name "$container" --restart unless-stopped \
  --read-only --tmpfs /tmp:rw,noexec,nosuid,nodev,size=16m \
  --cap-drop ALL --security-opt no-new-privileges \
  --memory 128m --memory-swap 128m --cpus 0.5 --pids-limit 64 \
  --ulimit nofile=128:128 \
  --mount "type=volume,source=$volume,target=/secrets,readonly" \
  -p "127.0.0.1:$port:31337" "$image" >/dev/null

echo "Warm Tofu is ready: nc 127.0.0.1 $port"
