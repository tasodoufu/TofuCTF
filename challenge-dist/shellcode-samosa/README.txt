Shellcode Samosa
=================

Local amd64 pwn challenge. The kitchen stores a recipe on the stack and then
serves it as executable code. Bring your own shellcode to read the hidden menu.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop
