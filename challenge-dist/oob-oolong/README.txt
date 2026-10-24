OOB Oolong
==========

Local amd64 pwn challenge. The tea-label tray has eight slots, but its index
check accidentally covers the callback stored just after the tray. Use the
out-of-bounds indexed write to replace the finish callback with the hidden
serving function.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
