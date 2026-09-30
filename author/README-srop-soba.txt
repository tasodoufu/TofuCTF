SROP Soba author notes
======================

Build the stripped artifact with:
  gcc -std=c11 -D_GNU_SOURCE -Wall -Wextra -Wpedantic -O0 -fno-stack-protector -fno-pie -no-pie -Wl,-z,relro,-z,now -Wl,-z,noexecstack author/srop-soba.c -o challenge-dist/srop-soba/srop-soba
  strip --strip-all challenge-dist/srop-soba/srop-soba

The exploit leaks the recipe buffer, triggers rt_sigreturn with a 15-byte
read, then restores open/read/write register states. It is for local testing
only and reads the flag path installed by run.sh.
