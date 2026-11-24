Prev-in-Use Poutine — House of Einherjar / off-by-null / backward consolidation / safe unlink

The order desk discloses the heap address of a large recipe card. Its final
byte reaches the following chunk's size field. Forge a predecessor chunk and
make the allocator's backward-consolidation link checks update the pantry's
approval gate. The target is local and intentionally built without PIE so the
global gate address can be found in the ELF.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: prev-in-use-poutine
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE / ASLR
