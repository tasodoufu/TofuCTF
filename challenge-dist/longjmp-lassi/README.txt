Longjmp Lassi
=============

Local amd64 pwn challenge. The kitchen stores its checkpoint's saved RIP in a
pointer-mangled jmp_buf. Inspect the encoded pointer, recover the pointer guard
from the known checkpoint address, encode the hidden recipe address, and restore.

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
