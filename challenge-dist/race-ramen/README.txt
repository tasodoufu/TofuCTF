Race Ramen
==========

Local amd64 pwn challenge. The ramen register checks two orders against the
same balance before applying either debit. Queue two 75-unit orders, then
settle them to reproduce the check-then-use race and reveal the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
