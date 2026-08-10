FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends gosu socat && rm -rf /var/lib/apt/lists/* && useradd -m ctf
COPY warm-tofu /home/ctf/warm-tofu
COPY entrypoint.sh /entrypoint.sh
RUN chown root:root /home/ctf/warm-tofu /entrypoint.sh && chmod 755 /home/ctf/warm-tofu /entrypoint.sh
EXPOSE 31337
ENTRYPOINT ["/entrypoint.sh"]
