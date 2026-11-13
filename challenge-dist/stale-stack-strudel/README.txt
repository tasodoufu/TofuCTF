Stale-Stack Strudel — uninitialized stack read / stale stack-slot disclosure

The ticket handler reuses a stack slot left behind by the previous function.
Observe the stale value and replay it to claim the secret recipe.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: stale-stack-strudel
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
