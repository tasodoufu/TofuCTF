Overread Pavlova — stack overread / PIE leak / ret2win

The tasting-note handler writes 80 bytes from a 64-byte stack card. The extra
bytes disclose the saved frame pointer and a PIE return address. Use that leak
to recover the randomized image base, then the second, oversized recipe card
can redirect execution to the hidden recipe function.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: overread-pavlova
Architecture: amd64
Mitigations: NX / Full RELRO / PIE / ASLR
