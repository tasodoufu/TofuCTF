Null-Byte Nougat
================

Local amd64 pwn challenge. A 16-byte tasting note is stored directly before
its approval byte. Entering exactly 16 non-newline bytes makes fgets place its
terminating NUL in the adjacent approval byte, clearing it and unlocking the
hidden recipe.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337 by default. For author-side
verification, run.sh also accepts PORT=<available-local-port>. The flag is
derived from this package's stripped binary and stored in a Docker volume;
the literal flag is not included in the distribution files.
