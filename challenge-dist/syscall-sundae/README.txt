Syscall Sundae
===============

Local amd64 pwn challenge. The sundae counter accepts an oversized recipe
card. Build a ROP chain that uses the supplied syscall; ret gadget to open
/flag, read it into scratch space, and write it to standard output.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
