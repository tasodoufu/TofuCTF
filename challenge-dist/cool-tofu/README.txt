Cool Tofu / TofuCTF
====================

Requirements: Docker, netcat

1. Start the local challenge service:
     chmod +x run.sh stop.sh
     ./run.sh

2. Analyze the included `cool-tofu` binary.

3. Connect locally:
     nc 127.0.0.1 31337

4. Retrieve the flag and submit it on TofuCTF.

Google login is optional. Without login, solved progress is stored in the
current browser; with login, it is also synced to the account.

The service listens on localhost only. run.sh does not contact an external
service. The flag is stored in a Docker volume, not in the extracted directory.
