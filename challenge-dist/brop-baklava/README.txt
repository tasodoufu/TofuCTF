BROP Baklava
=============

Local amd64 pwn challenge. The baklava counter behaves like a fork-per-order
service: a bad return address only closes that connection. Use the crash oracle
to identify the saved return-address offset, then build a blind ROP payload that
reaches the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
