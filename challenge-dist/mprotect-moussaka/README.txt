Mprotect Moussaka
=================

Local amd64 pwn challenge. The recipe is staged on a non-executable page,
but the order-card reader overflows its tray. Build a ROP chain that calls
mprotect to make the staging page executable, then transfer control to your
staged shellcode and read /flag.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
