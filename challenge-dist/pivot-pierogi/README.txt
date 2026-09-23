Pivot Pierogi
=============

The kitchen lets you place a long ROP recipe on a pastry board, then signs a
short order note with an oversized legacy field. Pivot the stack onto the
board and reach the hidden recipe.

Local use (Linux amd64 with Docker):

  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
