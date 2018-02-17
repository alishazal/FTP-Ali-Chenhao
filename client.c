// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
//#define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sd = 0, valread;
    //int PORT = 8080;
    struct sockaddr_in serv_addr;
    char *hello = "PASS 123\nUSER chen\n USER ali\n USER nabil\n USER yasir\nPASS 123\nPASS 321\nblah\nQUIT\nPASS 123\n";
    char buffer[1024] = {0};
    char cmd[256];
    char userIn[256];
    char msg[256];

    int PORT = atoi(argv[2]);
    sd = socket(AF_INET, SOCK_STREAM, 0);

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
    
    if (connect(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1){
    	printf("\nftp> ");
    	fgets(cmd, 256, stdin);
    	sscanf(cmd, "%s", userIn);

    	if (strcmp(userIn, "USER")==0){
    		send(sd , cmd , strlen(cmd) , 0 );
    		printf("Username sent\n");
    		valread = read(sd, buffer, 1024);
            //FILE *ss = fdopen(sd, "r");
            //fgets(msg, 256, ss);
            //buffer[valread] = '\0';
    		printf("%s\n", buffer);

    	}

    	if (strcmp(userIn, "PASS")==0){
    		send(sd , cmd , strlen(cmd) , 0 );
    		printf("Password sent\n");
    		valread = read(sd, buffer, 1024);
            buffer[valread] = '\0';
    		printf("%s\n", buffer);

    	}

    }
    //send(sd , hello , strlen(hello) , 0 );
    //printf("Hello message sent\n");
    //valread = read( sd , buffer, 1024);
    //printf("%s\n",buffer );
    return 0;
}
