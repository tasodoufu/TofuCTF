ABI Arancini
============

Local amd64 pwn challenge about seccomp architecture confusion. The filter
blocks native open/openat syscall numbers but does not validate the syscall
architecture. Use the stack overflow to build a ROP chain that invokes the
legacy i386 int 0x80 ABI to read the secret recipe.

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
