Buffered Biscotti — stdio/raw-read input desynchronization

The name is read with buffered fgets, then the recipe card is read with the
raw read(2) interface. Mixing these APIs on the same stream can strand bytes
that stdio read ahead in its private buffer. Send the name line first and wait
for the raw-card prompt before sending the overflowing card.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

Send the recipe-card payload when prompted. The challenge binds only to
127.0.0.1. Set PORT to another available localhost port when needed; the
container still listens on 31337.

Challenge ID: buffered-biscotti
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
