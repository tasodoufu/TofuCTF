Frame Tamale — saved-RBP overwrite / fake stack frame / leave; ret pivot

The order card is read into a 64-byte stack buffer, but the service always
accepts 88 bytes. The saved RBP and return address are therefore reachable.
First provide a 24-byte fake frame in writable .bss; then redirect the function
epilogue to a leave; ret gadget so the stack pivots into that frame. Keep the
AMD64 function-entry stack alignment in mind when arranging the next return.

Protocol: send exactly 24 raw bytes for the stage frame, followed by exactly
88 raw bytes for the order card.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

The service binds only to 127.0.0.1:31337 by default. Set PORT to another
available localhost port for author-side verification; the container still
listens on 31337. The flag is derived from the distributed binary and stored
in a Docker volume, not in the binary or image.

Challenge ID: frame-tamale
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
