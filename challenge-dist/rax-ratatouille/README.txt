RAX Ratatouille
===============

A local amd64 pwn challenge about ret2reg. The vulnerable order reader has a
64-byte stack buffer and overwrites its saved return address after 72 bytes.
After the read, RAX contains the address of the hidden recipe function. Find
the `jmp rax` gadget and redirect the saved return address to it; this uses the
existing register value rather than constructing a conventional ROP call.

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
