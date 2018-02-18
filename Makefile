all:
	gcc client.c -o FTPclient
	gcc server.c -o FTPserver
FTPclient: client.c
	gcc client.c -o FTPclient

FTPserver: server.c
	gcc server.c -o FTPserver
