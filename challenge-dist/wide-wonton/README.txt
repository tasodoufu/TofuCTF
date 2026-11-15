Wide Wonton — wide-character element-count overflow / adjacent authorization overwrite

The order label is converted from UTF-8 into wchar_t elements. Reverse the
stripped amd64 binary and follow the byte-size versus element-count mismatch
to alter the adjacent authorization value.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: wide-wonton
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE / ASLR
