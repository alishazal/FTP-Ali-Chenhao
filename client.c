#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sd = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char cmd[256];
    char userIn[256];
    char msg[256];

    //Takes port from argv
    int PORT = atoi(argv[2]);
    sd = socket(AF_INET, SOCK_STREAM, 0);

    //In case of socket creation error
    if (sd < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    //Connection failed
    if (connect(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    //After connection is established, main loop
    while(1){

        //Displays "ftp> " in the command line
    	printf("\nftp> ");

        //Gets command from user
    	fgets(cmd, 256, stdin);

        //Stores first word of the command
    	sscanf(cmd, "%s", userIn);

        //If command is "USER username"
    	if (strcmp(userIn, "USER")==0){

            //Sends command to server
    		send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
    		valread = read(sd, buffer, 1024);
            //Displays if the command was successfully executed
    		printf("%s\n", buffer);

    	}

        //If command is "PASS password"
    	else if (strcmp(userIn, "PASS")==0){

            //Sends command to server
    		send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
    		valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
            //Displays if the command was successfully executed
    		printf("%s\n", buffer);

    	}

        //If command is "PUT a existed file"
        else if (strcmp(userIn, "PUT")==0){

            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
            //Displays if the command was successfully executed
            printf("%s\n", buffer);

        }

        //If command is "GET a file"
        else if (strcmp(userIn, "GET")==0){

            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
            //Displays if the command was successfully executed
            printf("%s\n", buffer);

        }

        //If command is "CD ..."
        else if (strcmp(userIn, "CD")==0){

            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
            //Displays if the command was successfully executed
            printf("%s\n", buffer);

        }

        //If command is "LS ..."
        else if (strcmp(userIn, "LS")==0){

            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
            //Displays if the command was successfully executed
            printf("%s\n", buffer);

        }

        //If command is "PWD"
        else if (strcmp(userIn, "PWD")==0){

            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
            //Displays if the command was successfully executed
            printf("%s\n", buffer);

        }

        //Doesn't accept any other commands
        else {
            printf("An invalid ftp command.");
        }
    }

    return 0;
}
