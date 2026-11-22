Shadow-Space Shortbread — Microsoft x64 calling convention / ROP / stack overflow

The vulnerable order card overwrites a saved return address. The hidden menu
uses the Microsoft x64 ABI despite running in a Linux amd64 ELF: its first
argument arrives in RCX, and its caller-provided stack frame includes 32 bytes
of shadow space after the return address. Build a ROP chain that respects both
that convention and the stack alignment expected at function entry.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored in
a Docker volume, not in the binary or image.

Challenge ID: shadow-space-shortbread
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
