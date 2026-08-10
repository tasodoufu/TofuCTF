# TofuCTF

Personal, static pwn practice site for GitHub Pages.

## Preview

```sh
python3 -m http.server 8080
```

Open <http://localhost:8080>.

## Challenge model

The website is static. Each pwnable is downloaded and run locally with Docker,
then attacked through netcat. A short-lived launch token fetches an
account-specific flag from the Cloudflare Worker directly inside the container,
so neither the editor's file tree nor the public Pages repository contains the
answer. The Worker verifies submitted flags and stores solved progress in D1.

## Requirements

- Linux on amd64, or a compatible VM
- Docker
- netcat
