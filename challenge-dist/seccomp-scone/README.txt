Seccomp Scone
=============

Local amd64 pwn challenge. The kitchen installs a syscall allow-list before
reading a recipe card. Use the oversized card to reach the hidden recipe
handler while respecting the sandbox's tiny syscall menu.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
