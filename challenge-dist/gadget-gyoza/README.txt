Gadget Gyoza — ROP / multi-pop gadget side effects / AMD64 stack alignment

The order desk accepts a long recipe card. Inspect the available pop sequence,
account for every consumed stack word (including the extra register), and align
the target function's stack according to the AMD64 calling convention.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: gadget-gyoza
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
