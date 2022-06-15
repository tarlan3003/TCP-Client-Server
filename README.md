The aim of this project is to create TCP model for the communication between Client and Server. 
For running program you should enter in command line:
1. gcc server.c -o server
2. gcc client.c -o client (depending on the number of clients, client1, client2 ... can be used)


The server will only accept messages ending with \r\n. If such kind of message is entered, then the server will send 
+ACCEPTED message to the client if not -PROTOCOL ERROR will be sent. All the messages that are sent by the clients and server
will be stored in logfile.txt with the time that they have been sent and port number.
