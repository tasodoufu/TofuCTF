Stack Mignon
============

A local amd64 pwn challenge about the SysV AMD64 function-entry stack-alignment
rule. The vulnerable order reader overwrites its saved return address after 72
bytes. The hidden recipe gate accepts only the expected 16-byte alignment, so
chain a one-instruction `ret` shim before the target to adjust RSP by eight.

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
