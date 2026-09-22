Off-by-One Onigiri
===================

Local amd64 pwn challenge. The order note accepts one byte more than its buffer.
Use that off-by-one write to place the hidden seal and unlock the secret serving path.

Start:
  ./run.sh
Connect:
  nc 127.0.0.1 31337
Stop and remove the container/flag volume:
  ./run.sh stop
