Wraparound Waffle
=================

Local amd64 pwn challenge. A one-byte length check wraps around while the
original length still controls the read. Use the oversized recipe card to
replace the adjacent serving callback and reach the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
