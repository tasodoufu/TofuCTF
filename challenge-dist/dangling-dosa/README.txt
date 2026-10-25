Dangling Dosa
=============

Local amd64 pwn challenge. An order record is kept on a function's stack and
its address survives that function's return. A later function reuses the same
stack-frame slot for a 40-byte recipe card; the stale order pointer is then
used for an indirect call. Recover the non-PIE serving function and replace the
callback in the reused frame.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop

The service listens only on 127.0.0.1:31337 by default. The supplied run.sh
also accepts PORT=<available-local-port> for author-side testing. It derives
the local flag from this package's stripped binary and stores it in a Docker
volume; the literal flag is not included in the distribution files.
