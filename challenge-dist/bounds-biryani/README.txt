Bounds Biryani
==============

Local amd64 pwn challenge. The recipe index is missing a bounds check, so an
out-of-bounds read can disclose the PIE address of the special callback. The
order reader then accepts more than its 40-byte buffer, allowing a ret2win
chain once the address is known.

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
