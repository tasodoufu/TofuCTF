SROP Soba
=========

Local amd64 pwn challenge. The soba counter leaks the stack address of its
recipe card and accepts an oversized card. Use the leak to place sigreturn
frames on the stack, then build a sigreturn-oriented chain for open/read/write.
The first sigreturn is triggered by a 15-byte read; later frames restore the
registers needed for the next raw syscall.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
