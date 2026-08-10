Warm Tofu / TofuCTF
====================

Requirements: Docker, netcat

1. Start the service:
     ./run.sh

2. In another terminal, connect:
     nc localhost 31337

3. Analyze `warm-tofu` and retrieve the generated flag.

Stop with Ctrl-C. The flag is generated only inside the container and changes
each time run.sh starts, so it will not appear in your editor's file tree.
