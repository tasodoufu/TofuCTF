Struct-Stuffed Sarmale — aggregate-size input / intra-struct authorization overwrite

The kitchen asks for a 24-byte note. Inspect the stripped amd64 service and
figure out why the sealed recipe's approval can be changed without altering
control flow.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: struct-stuffed-sarmale
Architecture: amd64
Mitigations: NX / Stack Canary / Full RELRO / No PIE
