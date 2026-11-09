NUL-Byte Nachos
===============

A local amd64 pwn challenge about a mismatch between raw byte counts and
NUL-terminated C strings. The menu validates strlen(input), but copies the
full received byte count into a small stack note. An embedded NUL hides the
remaining bytes from validation.

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
