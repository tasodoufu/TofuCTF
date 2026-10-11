Fini Focaccia
=============

Local amd64 pwn challenge. The focaccia counter's signed recipe-slot check
forgets the lower bound, so a negative slot can overwrite a nearby exit-time
function pointer. Redirect the hidden final tray to the recipe reader.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
