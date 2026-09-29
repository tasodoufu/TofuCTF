Leak Lasagna
============

Local amd64 pwn challenge. The lasagna counter leaks the address of a libc
function and accepts an oversized recipe card. Use the leak to calculate the
libc base, then build a ret2libc chain to reach a shell and read /flag.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
