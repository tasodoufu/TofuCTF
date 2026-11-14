Scanf Spanakopita — scanf destination-width mismatch / function-pointer overwrite

The menu reads a signed 32-bit integer directly into the low half of a 64-bit
function pointer. Reverse the stripped amd64 binary, locate the hidden recipe
handler, and redirect the dispatch slot.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: scanf-spanakopita
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
