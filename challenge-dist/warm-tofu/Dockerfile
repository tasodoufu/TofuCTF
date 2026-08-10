FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends socat && rm -rf /var/lib/apt/lists/* && useradd -m ctf
COPY warm-tofu /home/ctf/warm-tofu
RUN chown root:root /home/ctf/warm-tofu && chmod 755 /home/ctf/warm-tofu
USER ctf
EXPOSE 31337
CMD ["socat", "TCP-LISTEN:31337,reuseaddr,fork", "EXEC:/home/ctf/warm-tofu,stderr"]

