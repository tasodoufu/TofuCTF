# TofuCTF

Personal, static pwn practice site for GitHub Pages.

## Preview

```sh
python3 -m http.server 8080
```

Open <http://localhost:8080>.

## Challenge model

The website is static. Challenge packages are downloaded from GitHub Pages and
run in restricted Docker containers on each player's own machine. Services bind
to localhost only. Flags are derived from each challenge artifact and installed
into Docker volumes locally; literal flag values are not included in run.sh.
The browser verifies their hashes locally. The Cloudflare Worker is only used
to sync signed-in users' progress; Google login remains optional.

## Requirements

- Linux on amd64, or a compatible VM
- Docker
- netcat
