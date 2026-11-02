BSS Borscht
===========

Local amd64 pwn challenge. A 24-byte global note is immediately followed by
an authorization field. Writing past the note changes data, not control flow,
so the usual control-flow mitigations do not prevent this data-only attack.

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
