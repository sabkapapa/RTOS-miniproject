# RTOS-miniproject
Instructions to execute 1to1 i.e. Phase1:
First install Pulseaudio library in your device and then run it in one of the terminals.
After that compile the 1to1 Voice Server and Client files using "gcc filename.c -lpthread -lpulse -lpulse-simple -o server/client.
Next execute the Server and Client using command: ./server 'port no.' & ./client 'IP address of self loop' 'port no.' 'id'.
Here, I have used 8080 as my port no. and the loop back address to be '127.0.0.1'.

