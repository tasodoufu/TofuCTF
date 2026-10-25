# The prebuilt ELF requires GLIBC_2.38; Debian trixie supplies glibc 2.41.
FROM debian:trixie-slim
RUN apt-get update \
 && apt-get install -y --no-install-recommends socat \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --create-home --uid 1000 ctf \
 && ln -s /secrets/flag /flag
COPY oob-oolong /home/ctf/challenge
RUN chown root:root /home/ctf/challenge && chmod 755 /home/ctf/challenge
USER ctf
EXPOSE 31337
ENTRYPOINT ["socat", "TCP-LISTEN:31337,reuseaddr,fork,max-children=16", "EXEC:/home/ctf/challenge,stderr"]
