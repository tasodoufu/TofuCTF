Type-Confused Cannoli
=====================

Local amd64 pwn challenge. The cannoli order can be viewed as either a note
or an action, but the note editor does not respect the selected type. Reuse
the note layout to replace the action callback and reach the hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's stripped binary and stores it in a Docker volume;
the literal flag is not included in the distribution files.
