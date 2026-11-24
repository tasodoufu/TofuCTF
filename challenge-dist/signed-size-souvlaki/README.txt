Signed-Size Souvlaki — signed-to-unsigned length conversion / stack buffer overflow / ret2win

The order counter rejects positive lengths above the card size, but accepts
negative signed lengths. The accepted value is converted through uint32_t and then widened to size_t for
read(), allowing an oversized order card to overwrite the saved return address.
Recover the hidden grill's address and redirect execution to it.

The first input is a signed 32-bit little-endian length; the second is the raw
order-card payload.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored in
a Docker volume, not in the binary or image.

Challenge ID: signed-size-souvlaki
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
