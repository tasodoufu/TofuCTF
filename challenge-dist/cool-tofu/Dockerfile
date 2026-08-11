FROM debian:bookworm-slim

RUN apt-get update \
 && apt-get install -y --no-install-recommends socat \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --create-home --uid 1000 ctf \
 && ln -s /secrets/flag /flag

COPY cool-tofu /home/ctf/challenge
COPY entrypoint.sh /entrypoint.sh
RUN chown root:root /home/ctf/challenge /entrypoint.sh \
 && chmod 755 /home/ctf/challenge /entrypoint.sh

USER ctf
EXPOSE 31337
ENTRYPOINT ["/entrypoint.sh"]
