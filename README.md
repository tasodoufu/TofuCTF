# TofuCTF

Personal, static pwn practice site for GitHub Pages.

## Preview

```sh
python3 -m http.server 8080
```

Open <http://localhost:8080>.

## Challenge model

The website is static. Each pwnable is downloaded and run locally with Docker,
then attacked through netcat. Flags are generated locally on every launch, so
the public Pages repository does not contain a reusable secret.

Because GitHub Pages has no server-side flag verifier, the submit dialog checks
the flag format and stores progress only in your browser. The actual goal is to
retrieve the generated flag from the local service with a working exploit.

## Requirements

- Linux on amd64, or a compatible VM
- Docker
- OpenSSL
- netcat
