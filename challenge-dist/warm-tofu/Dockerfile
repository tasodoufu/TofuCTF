FROM debian:bookworm-slim

RUN apt-get update \
 && apt-get install -y --no-install-recommends socat \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --create-home --uid 1000 ctf \
 && ln -s /secrets/flag /flag

COPY warm-tofu /home/ctf/challenge
RUN chown root:root /home/ctf/challenge \
 && chmod 755 /home/ctf/challenge

USER ctf
EXPOSE 31337
ENTRYPOINT ["socat", "TCP-LISTEN:31337,reuseaddr,fork,max-children=16", "EXEC:/home/ctf/challenge,stderr"]
