Realloc Roti — use-after-realloc / stale heap pointer

The note grows with realloc, but the old pointer survives. The freed chunk is
reused for a serving callback. Use the stale pointer to change the callback.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

Send exactly 64 bytes at the prompt. The challenge binds only to 127.0.0.1.
Set PORT to another available localhost port when needed; the container still
listens on 31337.

Challenge ID: realloc-roti
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
