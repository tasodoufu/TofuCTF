Unlink Ube
==========

Local amd64 pwn challenge. The order book uses a doubly linked list, but its
unlink operation trusts forged neighboring pointers. Overflow an order note,
turn the unlink into an arbitrary pointer write, and redirect the selected
recipe to the hidden one.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
