Warm Tofu / TofuCTF
====================

Requirements: Docker, netcat

1. Sign in to TofuCTF and click "Copy launch token" for Warm Tofu.

2. Start the service and paste the token when prompted:
     ./run.sh

3. In another terminal, connect:
     nc localhost 31337

4. Analyze `warm-tofu`, retrieve the flag, and submit it on TofuCTF.

Stop with Ctrl-C. The account-specific flag is fetched only inside the
container. The launch token expires after 15 minutes and is not itself a flag.
