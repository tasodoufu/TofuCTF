JOP Jambalaya
=============

Local amd64 pwn challenge. The jambalaya counter routes every step through an
indirect jump. Overflow the recipe card, seed the provided dispatch gadgets,
and chain a call to the hidden recipe without relying on ordinary ROP returns.
The binary is non-PIE, so the fixed gadget addresses can be used after you
understand the supplied artifact.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337. The supplied run.sh derives the
local flag from this package's binary and stores it in a Docker volume; the
literal flag is not included in the distribution files.
