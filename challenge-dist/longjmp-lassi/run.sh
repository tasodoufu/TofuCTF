#!/bin/sh
set -eu
cd "$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
slug=longjmp-lassi
image="tofuctf/$slug:local"
container="tofuctf-$slug"
volume="tofuctf-$slug-flag"
port="${PORT:-31337}"
command -v docker >/dev/null 2>&1 || { echo "Docker is required." >&2; exit 1; }
case "${1:-start}" in
  start) ;;
  stop)
    docker rm -f "$container" >/dev/null 2>&1 || true
    docker volume rm "$volume" >/dev/null 2>&1 || true
    echo "Longjmp Lassi stopped."
    exit 0
    ;;
  *) echo "Usage: $0 [start|stop]" >&2; exit 2 ;;
esac
case "$port" in *[!0-9]*|'') echo "PORT must be a numeric localhost port." >&2; exit 2 ;; esac
if [ "$port" -lt 1024 ] || [ "$port" -gt 65535 ]; then echo "PORT must be between 1024 and 65535." >&2; exit 2; fi
command -v sha256sum >/dev/null 2>&1 || { echo "sha256sum is required." >&2; exit 1; }
if docker container inspect "$container" >/dev/null 2>&1; then
  echo "Refusing to replace existing container $container; run ./run.sh stop first." >&2
  exit 1
fi
if docker volume inspect "$volume" >/dev/null 2>&1; then
  echo "Refusing to replace existing volume $volume; run ./run.sh stop first." >&2
  exit 1
fi
binary_hash="$(sha256sum "./$slug" | awk '{print $1}')"
flag_hex="$(printf '%s' "tofuctf-local-v1:$slug:$binary_hash" | sha256sum | cut -c1-32)"
flag="TofuCTF{$flag_hex}"
unset binary_hash flag_hex
docker build -t "$image" .
docker volume create "$volume" >/dev/null
printf '%s\n' "$flag" | docker run --rm -i --user 0 --entrypoint sh --mount "type=volume,source=$volume,target=/secrets" "$image" -c 'umask 077; cat > /secrets/flag; chown 1000:1000 /secrets/flag; chmod 0400 /secrets/flag'
unset flag
docker run -d --name "$container" --restart unless-stopped \
  --read-only --tmpfs /tmp:rw,noexec,nosuid,nodev,size=16m \
  --cap-drop ALL --security-opt no-new-privileges \
  --memory 128m --memory-swap 128m --cpus 0.5 --pids-limit 64 --ulimit nofile=128:128 \
  --mount "type=volume,source=$volume,target=/secrets,readonly" \
  -p "127.0.0.1:$port:31337" "$image" >/dev/null
echo "Longjmp Lassi is ready: nc 127.0.0.1 $port"
