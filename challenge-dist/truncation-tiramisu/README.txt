Truncation Tiramisu
====================

Local amd64 pwn challenge. The dessert ledger narrows a requested recipe
length to one byte. Supply a value whose low byte is long enough to pass the
32-byte recipe tray and replace its handler with the hidden recipe function.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
