Readv Risotto — readv iovec-length mismatch / adjacent callback overwrite

The card-length protocol limit is 64 bytes, but the order note is only 32 bytes.
The program passes the accepted length directly as readv()'s iov_len, allowing
bytes to continue into the adjacent dispatch callback. Redirect it to the hidden
recipe function.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: readv-risotto
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
Protocol: enter a decimal card length from 1 to 64, then send that many bytes.
