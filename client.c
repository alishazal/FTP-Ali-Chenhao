#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    struct sockaddr_in data_addr; // Address for receiving data
    int data_sd = 0; // Socket for receiving data
    int sd = 0, valread, addr_len;
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

    addr_len = sizeof(address);

    if (getsockname(sd , (struct sockaddr*)&address, &addr_len) == -1) {
        perror("Cannot read socket information.\n");
        return -1;
    }

    data_addr = address;
    data_addr.sin_port = htons(ntohs(address.sin_port) + 1);

    if( (data_sd = socket(AF_INET , SOCK_STREAM , 0)) == 0) { // Create new socket
        perror("data socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    if (bind(data_sd, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) { // Bind socket to data port
        perror("bind failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(data_sd, 3) < 0) { // Listens to server
        perror("listen  failed\n");
        exit(EXIT_FAILURE);
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
            buffer[valread] = '\0';
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
            addr_len = sizeof(data_addr);
            int new_data_sd = 0;
            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';

            //Displays if the command was successfully executed
            //printf("Read from control socket: %s\n", buffer);
            // Ali: please add error handling here, if buffer reads "wrong command usage" then declare error and skip the next part

            if ((new_data_sd = accept(data_sd, (struct sockaddr *)&data_addr, (socklen_t*)&addr_len)) == 0) {
                perror("Failure to accept connection ");
                continue;
            }

            valread = recv(new_data_sd, buffer, 1024, MSG_WAITALL); // Read data until socket is closed by the server
            buffer[valread] = '\0';
            printf("%s\n", buffer);
            close(new_data_sd);
        }

        //If command is "PWD"
        else if (strcmp(userIn, "PWD")==0){
            addr_len = sizeof(data_addr);
            int new_data_sd = 0;
            //Sends command to server
            send(sd , cmd , strlen(cmd) , 0 );
            //Reads reply from socket
            valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';

            //Displays if the command was successfully executed
            //printf("Read from control socket: %s\n", buffer);
            // Ali: please add error handling here, if buffer reads "wrong command usage" then declare error and skip the next part

            if ((new_data_sd = accept(data_sd, (struct sockaddr *)&data_addr, (socklen_t*)&addr_len)) == 0) {
                perror("Failure to accept connection ");
                continue;
            }

            valread = recv(new_data_sd, buffer, 1024, MSG_WAITALL); // Read data until socket is closed by the server
            buffer[valread] = '\0';
            printf("%s\n", buffer);
            close(new_data_sd);
        }

        //Doesn't accept any other commands
        else {
            printf("An invalid ftp command.");
        }
    }

    close(sd);
    close(data_sd);

    return 0;
}
