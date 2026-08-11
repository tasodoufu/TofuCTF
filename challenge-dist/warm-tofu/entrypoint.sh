#!/bin/sh
set -eu
exec socat TCP-LISTEN:31337,reuseaddr,fork,max-children=16 EXEC:/home/ctf/challenge,stderr
