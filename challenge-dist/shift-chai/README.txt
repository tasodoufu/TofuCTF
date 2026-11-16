Shift-Chai — x86 masked shift count / off-by-one range check

The chai seal is controlled by a high-bit marker. Inspect the stripped amd64
binary and reason about how the processor interprets the accepted shift count.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: shift-chai
Architecture: amd64
Mitigations: NX / Stack Canary / Full RELRO / No PIE
