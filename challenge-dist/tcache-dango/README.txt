Tcache Dango
============

Local amd64 pwn challenge. The dango shop returns a tray but forgets to clear
its pointer. Reuse that stale tray to turn a double-free into a tcache poison,
then redirect the serving callback.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
