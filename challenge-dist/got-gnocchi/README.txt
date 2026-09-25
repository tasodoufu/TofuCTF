GOT Gnocchi
===========

Local amd64 pwn challenge. The gnocchi shop accepts a negative recipe-slot
index and writes a handler address before the action table. Use the resulting
GOT overwrite to redirect the closing action to the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
