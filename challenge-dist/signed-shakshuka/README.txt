Signed Shakshuka — signed-to-unsigned conversion / oversized read / data-only authorization overwrite

A signed 16-bit recipe-note length is checked against the note capacity, then
converted to uint16_t and widened to size_t for read(). Trace what a negative
length becomes, and notice the authorization field immediately after the note.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored in
a Docker volume, not in the binary or image.

Challenge ID: signed-shakshuka
Architecture: amd64
Mitigations: NX / Stack Canary / Full RELRO / PIE / ASLR
