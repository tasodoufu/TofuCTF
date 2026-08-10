# TofuCTF

Personal, static pwn practice site for GitHub Pages.

## Preview

```sh
python3 -m http.server 8080
```

Open <http://localhost:8080>.

## Challenge model

The website is static. Challenge binaries are downloaded from GitHub Pages and
the pwn services run in restricted rootless containers on the TofuCTF host.
Players connect over netcat. Fixed flags stay on the server; the Cloudflare
Worker verifies submissions and stores signed-in users' progress in D1.

## Requirements

- Linux on amd64, or a compatible VM
- netcat
