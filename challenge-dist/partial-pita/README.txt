Partial Pita
============

Local amd64 pwn challenge about a 16-bit partial overwrite of a saved return
address. The binary is PIE and ASLR remains enabled. Its ELF load segments use
64 KiB alignment, so redirect the return into the hidden recipe function by
changing only the low two bytes while leaving the randomized high bytes intact.

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
