#!/usr/bin/env sh
set -eu

# Generate the flag inside the container so editors on the host cannot reveal it.
umask 077
flag_hex="$(od -An -N16 -tx1 /dev/urandom | tr -d ' \n')"
printf 'TofuCTF{%s}\n' "$flag_hex" > /flag
chown root:ctf /flag
chmod 440 /flag

exec gosu ctf socat TCP-LISTEN:31337,reuseaddr,fork EXEC:/home/ctf/warm-tofu,stderr
