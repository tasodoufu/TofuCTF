CSU Churro
==========

Local amd64 pwn challenge. The churro counter has a ret2csu-style pair of
shortcuts: one loads six registers and the other moves three of them into the
first arguments before making an indirect call. The order card is only 64
bytes, but the reader accepts a much larger card. Recover the dispatch target
and the required order values, then use the register-loading sequence to reach
the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
