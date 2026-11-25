Alloca Alfajor — alloca size mismatch / dynamic stack-buffer overflow / ret2win

The order desk reserves the requested number of bytes on the stack with
alloca(), but then reads a fixed 128-byte recipe card. Work out where the
saved return address lies relative to the dynamic allocation and redirect it
to the hidden filling function. A one-instruction ret shim is provided for
SysV AMD64 stack alignment.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: alloca-alfajor
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
Protocol: enter a decimal capacity from 16 to 128, then send exactly 128 card bytes.
