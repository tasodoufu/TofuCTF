Fastbin Falafel
================

Local amd64 pwn challenge. The falafel counter returns a tray to glibc's
fastbin more than once and still lets the stale pointer be edited. Use the
duplicate entry and safe-linking calculation to redirect the next allocation
to the callback slot, then call the hidden recipe reader.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
