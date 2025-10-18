#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BACKLOG 10
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servAddr, clientAddr;

    if (argc != 3)
    {
        printf("Usage: %s <Port_Number> <Directory_Name>\n", argv[0]);
        exit(1);
    }

    int SERV_PORT = atoi(argv[1]);
    char *storageDir = argv[2];

    printf("Server starting...\n");
    printf("Port: %d\n", SERV_PORT);
    printf("Storage directory: %s\n", storageDir);

    // Step 1: Construct socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("Error: Cannot create socket");
        exit(1);
    }

    // Step 2: Bind address to socket
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Error: Cannot bind socket");
        close(listenfd);
        exit(1);
    }

    // Step 3: Listen request from client
    if (listen(listenfd, BACKLOG) < 0)
    {
        perror("Error: Cannot listen");
        close(listenfd);
        exit(1);
    }

    printf("Server started successfully!\n");
    printf("Listening on port %d...\n", SERV_PORT);

    // Step 4: Communicate with client
    int clientAddrLen = sizeof(clientAddr);

    while (1)
    {
        // Accept request
        connfd = accept(listenfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen);
        if (connfd < 0)
        {
            perror("Error: Cannot accept connection");
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(clientAddr.sin_addr),
               ntohs(clientAddr.sin_port));

        // send welcome message (test)
        char *welcomeMsg = "+OK Welcome to file server";
        int sendBytes = send(connfd, welcomeMsg, strlen(welcomeMsg), 0);
        if (sendBytes < 0)
        {
            perror("Error: Cannot send welcome message");
        }
        else
        {
            printf("Sent welcome message to client [%s:%d]\n",
                   inet_ntoa(clientAddr.sin_addr),
                   ntohs(clientAddr.sin_port));
        }

        // Close connection with client (temporarily, for testing)
        close(connfd);
    }

    // Never reached
    close(listenfd);
    return 0;
}
