# socket
Implemented a communication model including a single client, a monitor, a main server side, two backend storage servers and a backend computing server.
<br>
client.c is a client model<br>
monitor.c is a monitor model<br>
servera.c is the backend-server A model<br>
serverb.c is the backend-server B model<br>
serverc.c is the caculating server model<br>
aws.c is the main AWS server model<br>

In terminalrun makefile, then make serverA, make serverB, make server C, make aws, make monitor,<br>
at last start the client as “./client <LINK_ID> <SIZE> <POWER>" eg:"./client 4 1000 3"
  <br>
  The unit of file size is in bits, the input signal power is in unit dBm
