Vtable Vareniki
===============

Local amd64 pwn challenge. The vareniki order card is copied over a C++
object whose first field is a virtual-table pointer. Redirect that pointer
to the nearby fake table, then let virtual dispatch call the hidden recipe.
The binary is non-PIE, so the fixed table address can be used after you
understand the object layout and the vtable call sequence.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
