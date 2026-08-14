UAF Udon — use-after-free callback hijack

Learning theme: dangling heap objects and function-pointer reuse.

Start the local service:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove its container and flag volume:
  ./run.sh stop

This is a self-contained amd64 ELF challenge for local CTF practice.
