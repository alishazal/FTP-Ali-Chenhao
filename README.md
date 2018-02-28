# File Transfer Protocol Implementation - Computer Networks
Project 1: A File Transfer Protocol for Prof. Yasir Zaki's class Computer Networks by Ali Shazal and Chenhao Mark Xu.

## Useful Links 
https://www.geeksforgeeks.org/socket-programming-cc/

## Checkpoint I:

Please "make" the FTPclient and FTPserver executable.
Run the server by simply writing in the command line "./FTPserver".
To run the client, write in the command line "./FTPclient 127.0.0.1 8080".
We have currently made two users "chen" and "ali". The password for chen is "123" and for ali is "321".

# Final Submission:
 - ## How To Use
The client and server code is in the .c files. Use Makefile to build the FTPclient and FTPserver executables. Run the server by simply writing in the command line "./FTPserver". To run the client, write in the command line "./FTPclient 127.0.0.1 8080". Then, use the commands explained below.

- ## Client Design
We implemented the client side using a while loop. "ftp> " is printed out on every line. The code fgets the input by the user and compares the first word to see if it is a command. If it is, the command gets compared to our own list of commands using several if statements. None of the commands is initiated until the user authenticates. 

1. ### USER username
The username specified in the command is sent to the server to be compared if it exists. The server's reply is printed on the client side.

2. ### PASS password
If the username exists, server sends a message to client prompting it to input the password corresponding to the username. The password input is then sent to the server to check for its validity.

3. ### PUT filename
This command is used to upload a file named filename from the current client directory to the current server directory. When user "PUT"s something, first the number of arguments are checked. Then, the code checks if the file exists on the client side. Then the command is sent to the server. A seperate data connection is established to deliver the file from client to server using fwrite. Multiple read and write cycles are required for large files. The connection is closed at the end of file transfer.

4. ### GET filename
GET is similar to PUT, difference being just that it's from server to client. This command is used to download a file named filename from the current server directory to the current client directory. Similar to PUT, a new separate TCP
connection to the server is be opened for the data transfer.

5. ### !CD ...
This command is used to change the current client directory. This has been implemented through the chdir() function. 

6. ### !LS ...
This command is used to list all the files under the current client directory. This was implemented using the sysmtem() function. 

7. ### !PWD
This command displays the current client directory. Like !LS, it was implemented using the system() function.

8. ### QUIT
This command quits the FTP session and closes the control TCP connection

9. ### Other commands
No other commands are accepted. An "Invalid FTP Command" error is thrown to the client in this case.

- ## Server Design
We implemented the server side using several functions to handle different requests, and a while loop in main() to drive the daemon. Every command except USER and PASS requires authentication.

1. ### USER username
When server receives this command, it passes it to the processUser() function, which uses another function findUser() to go over the clients stored in the server. It then prints the username found/not-found message on both the server and client side.

2. ### PASS password
This command is passed by the server to processPass(), which checks if the username was set first and then checks if the password is correct. The corresponding message is printed on server and client sides.

3. ### PUT filename
This command causes the server to start a seperate data connection and create a file called filename on its directory. Read and write cycles through recv() are run to recieve the file sent by client. 

4. ### GET filename
Again, a seperate data connection is started by the server by this command. Server checks if the file specified by the user actually exists. If it does, read and send cycles are run to send the file to client.

5. ### CD ...
This command is used to change the current server directory. This has been implemented through the chdir() function. 

6. ### LS ...
This command is used to list all the files under the current server directory. CHENHAO

7. ### PWD
This command displays the current server directory. CHENHAO

8. ### QUIT
This command quits the FTP session and closes the control TCP connection

9. ### Other commands
No other commands are accepted. An "Invalid FTP Command" error is thrown to the client in this case.

- ## Select and Context Switching
CHENHAO
