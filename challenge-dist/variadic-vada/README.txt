Variadic Vada — AMD64 SysV variadic-call ABI

A recipe-card overflow gives control of RIP. The hidden serving gate models a variadic function: the first integer ellipsis argument is passed in RSI, while AL reports how many vector registers carry variadic floating-point arguments. This gate expects an integer-only call, so AL must be zero. Build a small ROP chain that supplies the marker in RSI, clears RAX, and returns to the gate.

Local use:
  ./run.sh
  nc 127.0.0.1 31337
  ./run.sh stop

Send the recipe-card payload when prompted. The challenge binds only to 127.0.0.1. Set PORT to another available localhost port when needed; the container still listens on 31337.

Challenge ID: variadic-vada
Architecture: amd64
Mitigations: NX / Full RELRO / No PIE
