Seventh-Arg Arepa
=================

A local amd64 pwn challenge about the SysV AMD64 calling convention. The
first six integer arguments are passed in registers, but the seventh is read
from the stack. Use the stack overflow to return to the gate with its expected
seventh argument laid out after the return address.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337 by default. For author-side
verification, run.sh also accepts PORT=<available-local-port>. The flag is
deterministically derived from this package's stripped binary and stored in a
Docker volume; the literal flag is not included in the distribution files.
