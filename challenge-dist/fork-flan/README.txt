Fork Flan
=========

Local amd64 pwn challenge. The flan counter forks a fresh order attempt,
so a failed child reveals only whether its stack canary guess was accepted.
Recover the canary one byte at a time, then return to the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
