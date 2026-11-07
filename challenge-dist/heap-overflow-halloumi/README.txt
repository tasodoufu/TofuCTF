Heap Overflow Halloumi
======================

A local amd64 pwn challenge about integer multiplication overflow and a heap
buffer overflow. The batch count is multiplied by eight, then narrowed to a
16-bit allocation size, while the subsequent read still uses the full size.
The adjacent serving-station callback can be overwritten to call the hidden
recipe function.

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
