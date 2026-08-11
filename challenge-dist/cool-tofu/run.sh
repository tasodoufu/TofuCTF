#!/bin/sh
set -eu

image=tofuctf/cool-tofu:local
container=tofuctf-cool-tofu
volume=tofuctf-cool-tofu-flag
port="${PORT:-31337}"
flag='TofuCTF{d772673bf00aedf93d2f6ed17d5cc6df}'

command -v docker >/dev/null 2>&1 || { echo "Docker is required." >&2; exit 1; }

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

echo "Cool Tofu is ready: nc 127.0.0.1 $port"
