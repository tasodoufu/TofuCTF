Endian Eclair — network/host byte-order confusion / stack buffer overflow / ret2win

The service validates a recipe-card length after converting it from network byte
order, but the copy uses the unconverted value. Compare the accepted length with
the actual number of bytes the handler reads.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: endian-eclair
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
