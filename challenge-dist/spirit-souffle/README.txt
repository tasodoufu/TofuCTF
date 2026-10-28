Spirit Souffle
==============

Local amd64 pwn challenge. The kitchen keeps a forged 0x40-byte chunk in a
writable serving tray. Return and reclaim it, then redirect its callback to the
special recipe. Study the fake chunk header and the allocator's checks.

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
