Top-Chunk Tart
==============

Local amd64 pwn challenge. A tiny teaching allocator trusts its top-chunk
size after one recipe-card edit runs eight bytes past its allocation. Corrupt
the size, steer later allocations through the arena metadata, and replace the
serving callback. This models top-chunk size corruption and allocator
steering in an intentionally small allocator; it is not a system-heap exploit.

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
