Red-Zone Zongzi
================

Local amd64 pwn challenge. The input routine is a leaf function that uses the
AMD64 ABI's 128-byte red zone below RSP as its buffer. It reads past that area
and can overwrite the saved return address. The red zone is a calling-convention
scratch area, not a memory-safety boundary.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337 by default. For author-side
verification, run.sh also accepts PORT=<available-local-port>. The flag is
deterministically derived from this package's stripped binary and stored in a
Docker volume; the literal flag is not included in the distribution files.
