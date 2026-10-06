Resolve Ravioli
===============

Local amd64 pwn challenge. The ravioli counter accepts a stack overflow and
leaves the dynamic linker with no recipe for `system` in the imported table.
Build a ret2dlresolve chain that invokes `system("cat /flag")` without relying
on a libc address leak.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
