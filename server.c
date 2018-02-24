#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <dirent.h> // Directory stuffs

#define TRUE   1
#define FALSE  0
#define PORT 8080
#define DATA_PORT 8081
#define max_clients 50
#define BUF_SIZE 1024

// Based in part on code found on geeksforgeeks.com

void sendMsg(int sd, const char* msg) {
    if (send(sd, msg, strlen(msg), 0) != strlen(msg)) {
        perror("send");
    }
}

int findUser(const char* uname, char** usernames) {
    for (int i=0; i<max_clients; i++) {
        if (usernames[i] != NULL && strcmp(uname, usernames[i]) == 0) return i;
    }
    return -1;
}

void processUser(const char* uname, char** usernames, int sd, int sd_index, int* sd2user) {
    int result = findUser(uname, usernames);
    if (result == -1) {
        printf("Username %s does not exist\n", uname);
        sendMsg(sd, "Username does not exist");
    } else {
        printf("Username %s ok, password required\n", uname);
        sd2user[sd_index] = result;
        sendMsg(sd, "Username OK, password required");
    }
}

void processPass(const char* pass, char** passwords, int sd, int sd_index, int* sd2user, int* authenticated) {
    int user_index = sd2user[sd_index];
    if (user_index == -1) {
        printf("set USER first\n");
        sendMsg(sd, "set USER first");
    } else {
        if (passwords[user_index] != NULL && strcmp(pass, passwords[user_index]) == 0) {
            authenticated[sd_index] = 1;
            printf("Authentication complete\n");
            sendMsg(sd, "Authentication complete");
        } else {
            printf("wrong password\n");
            sendMsg(sd, "wrong password");
        }
    }
}

int openDataSocket(int sd, struct sockaddr_in src_addr) {
    // Open a data connection connection to user and returns the socket descriptor
    struct sockaddr_in user_addr; // Address of user control channel connection
    struct sockaddr_in target_addr; // Target address to connect to
    int addr_len = sizeof(user_addr);
    int new_sd;
    int opt = TRUE;
    if (getpeername(sd , (struct sockaddr*)&user_addr, &addr_len) == -1) {
        perror("Cannot read socket information.\n");
        return -1;
    }
    target_addr = user_addr;
    target_addr.sin_port = htons(ntohs(user_addr.sin_port) + 1);
    /*printf("User connection: ip %s , port %d \n" ,
                          inet_ntoa(user_addr.sin_addr) , ntohs(user_addr.sin_port));
    printf("Target connection: ip %s , port %d \n" ,
                          inet_ntoa(target_addr.sin_addr) , ntohs(target_addr.sin_port));*/

    if( (new_sd = socket(AF_INET , SOCK_STREAM , 0)) == 0) { // Create new socket
        perror("socket creation failed\n");
        return -1;
    }

    if(setsockopt(new_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, // Needs to set SO_REUSEADDR to allow immediate re-bind
          sizeof(opt)) < 0 ) {
        perror("setsockopt");
        return -1;
    }

    if (bind(new_sd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) { // Bind socket to source port
        perror("bind failed\n");
        return -1;
    }

    if (connect(new_sd, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) { // Connect to target
        perror("Connection Failed \n");
        return -1;
    }

    printf("Connected to client data channel. \n");

    return new_sd;
}

void processLS(char* command, int sd, struct sockaddr_in src_addr) {
    FILE *fp;
    int max = 256;
    char path[max];

    int data_sd = openDataSocket(sd, src_addr);

    if (data_sd == -1) {
        perror("Data channel connection failed\n");
        return;
    }

    for (int i=0; i<strlen(command) - 1; i++) {
        if (command[i] == 'L' && command[i+1] == 'S') {
            command[i] = 'l';
            command[i+1] = 's';
            break;
        }
    }

    fp = popen(command, "r");
    if (fp ==NULL) {
        perror("ls failed\n");
        sendMsg(sd, "wrong command usage!\n");
    } else {
        sendMsg(sd, "successfully executed!\n");
        while (fgets(path, max, fp) != NULL) {
            sendMsg(data_sd, path);
            printf("%s\n", path);
        }
    }

    pclose(fp);

    close(data_sd);
}

void processPWD(char* command, int sd, struct sockaddr_in src_addr) {
    FILE *fp;
    int max = 256;
    char path[max];

    int data_sd = openDataSocket(sd, src_addr);

    if (data_sd == -1) {
        perror("Data channel connection failed\n");
        return;
    }

    for (int i=0; i<strlen(command)-2; i++) {
        if (command[i] == 'P' && command[i+1] == 'W' && command[i+2] == 'D') {
            command[i] = 'p';
            command[i+1] = 'w';
            command[i+2] = 'd';
            break;
        }
    }

    for (int i=0; i<strlen(command); i++) {
        if (command[i] == 'L' && command[i+1] == 'S') {
            command[i] = 'l';
            command[i+1] = 's';
            break;
        }
    }

    fp = popen(command, "r");
    if (fp ==NULL) {
        perror("ls failed\n");
        sendMsg(sd, "wrong command usage!\n");
    } else {
        sendMsg(sd, "successfully executed!\n");
        while (fgets(path, max, fp) != NULL) {
            sendMsg(data_sd, path);
            printf("%s\n", path);
        }
    }

    pclose(fp);

    close(data_sd);
}

void processGet(char* filename, int sd, struct sockaddr_in src_addr, char* buffer) {
    FILE *fp = fopen(filename, "r");

    int data_sd = openDataSocket(sd, src_addr);

    if (data_sd == -1) {
        perror("Data channel connection failed\n");
        return;
    }

    if (!fp) {
        perror("File does not exist.");
        sendMsg(sd, "nonexisted\n");
        close(data_sd);
        return;
    } 

    sendMsg(sd, "existed");

    int valread;

    do { // Big files may require multiple reads
        valread = fread(buffer, sizeof(char), BUF_SIZE, fp);
        if (valread > 0) {
            buffer[valread] = '\0';
            sendMsg(data_sd, buffer);
            printf("read and send cycle\n");
        }
    } while(valread == BUF_SIZE);
    
    fclose(fp);
    close(data_sd);
}

void processPut(char* filename, int sd, struct sockaddr_in src_addr, char* buffer) {

    int data_sd = openDataSocket(sd, src_addr);

    if (data_sd == -1) {
        perror("Data channel connection failed\n");
        return;
    }

    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        perror("Failed to write file");
        close(data_sd);
        return;
    }

    int valread;

    do { // Big files may require multiple reads
        valread = recv(data_sd, buffer, BUF_SIZE, MSG_WAITALL); 
        if (valread > 0) {
            buffer[valread] = '\0';
            fprintf(fp, "%s", buffer);
            printf("read and write cycle\n");
        }
    } while(valread == BUF_SIZE);

    printf("Finshed reading");

    fclose(fp);
    close(data_sd);
}


void parseMsg(char* buffer, int sd, int sd_index, int* sd2user, int* authenticated, char** usernames, 
        char** passwords, int* client_socket, struct sockaddr_in src_addr) {
    char *line, ins[50];
    int c;
    int connected = 1; // If the socket is still connected
    line = strtok(strdup(buffer), "\n");
    while (line != NULL && connected) {
        c = sscanf(line, "%50s", ins); // Look for instruction
        if (c == 1) {
            if (strcmp(ins, "USER") == 0) {
                char arg[256];
                c = sscanf(line, "%*s %256s", arg);
                if (c == 1) {
                    printf("User %s wants to log in. \n", arg);
                    processUser(arg, usernames, sd, sd_index, sd2user);
                } else {
                    sendMsg(sd, "Wrong number of arguments. \n");
                }
            } else if (strcmp(ins, "PASS") == 0) {
                char arg[256];
                c = sscanf(line, "%*s %256s", arg);
                if (c == 1) {
                    printf("Password: %s \n", arg);
                    processPass(arg, passwords, sd, sd_index, sd2user, authenticated);
                } else {
                    sendMsg(sd, "Wrong number of arguments. \n");
                }
            } else if (strcmp(ins, "QUIT") == 0) {
                //Close the socket and mark as 0 in list for reuse
                close( sd );
                client_socket[sd_index] = 0;
                sd2user[sd_index] = -1; //dissociate user from connection
                authenticated[sd_index] = 0; //reset authentication status
                connected = 0; // So that the server stops reading messages
            } else if (strcmp(ins, "PUT") == 0) {
                if (!authenticated[sd_index]) {
                    sendMsg(sd, "Authenticate first");
                    return;
                }
                char arg[256];
                c = sscanf(line, "%*s %256s", arg);
                if (c == 1) {
                    printf("File: %s \n", arg);
                    processPut(arg, sd, src_addr, buffer);
                } else {
                    sendMsg(sd, "Wrong number of arguments. \n");
                }
            } else if (strcmp(ins, "GET") == 0) {
                if (!authenticated[sd_index]) {
                    sendMsg(sd, "Authenticate first");
                    return;
                }
                char arg[256];
                c = sscanf(line, "%*s %256s", arg);
                if (c == 1) {
                    printf("File: %s \n", arg);
                    processGet(arg, sd, src_addr, buffer);
                } else {
                    sendMsg(sd, "Wrong number of arguments. \n");
                }
            } else if (strcmp(ins, "LS") == 0) {
                if (!authenticated[sd_index]) {
                    sendMsg(sd, "Authenticate first");
                    return;
                }

                processLS(line, sd, src_addr);

            } else if (strcmp(ins, "PWD") == 0) {
                if (!authenticated[sd_index]) {
                    sendMsg(sd, "Authenticate first");
                    return;
                }
                processPWD(line, sd, src_addr);

            } else if (strcmp(ins, "CD") == 0) {
                if (!authenticated[sd_index]) {
                    sendMsg(sd, "Authenticate first");
                    return;
                }
            } else {
                sendMsg(sd, "Invalid FTP command: ");
                printf("Invalid command: %s\n", line);
                //sendMsg(sd, line);
            }
        }
        line = strtok(NULL, "\n");
    }
}




int main(int argc , char *argv[])
{

    char* usernames[max_clients] = {"chen", "ali"};
    char* passwords[max_clients] = {"123", "321"};
    int user_count = 2;


    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[max_clients] ,
          activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address, data_addr;

    char buffer[BUF_SIZE + 1];  //data buffer of 2K

    //set of socket descriptors
    fd_set readfds;

    //welcome message
    char *message = "Welcome to Ali and Chen's FTP server! \n";

    int authenticated[max_clients]; //map of whether a connection has authenticated
    int sd2user[max_clients]; //map from socket to user
    char* sd2dir[max_clients]; //map from socket to directory


    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
        authenticated[i] = 0;
        sd2user[i] = -1;
        sd2dir[i] = NULL;
    }

    for (i = user_count; i<max_clients; i++) {
        usernames[i] = NULL;
        passwords[i] = NULL;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    data_addr = address;
    data_addr.sin_port = htons( DATA_PORT );

    //bind the socket to localhost port 8080
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" ,
                   new_socket , inet_ntoa(address.sin_addr) , ntohs
                  (address.sin_port));
            /*
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }

            puts("Welcome message sent successfully");
            */
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 2048)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                    sd2user[i] = -1; //dissociate user from connection
                    authenticated[i] = 0; //reset authentication status
                }

                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    //openDataSocket(sd, data_addr);
                    parseMsg(buffer, sd, i, sd2user, authenticated, usernames, passwords, client_socket, data_addr); //process msg
                    //send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }

    return 0;
}
