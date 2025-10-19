#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

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

    // Check and create storage directory if it doesn't exist
    struct stat st = {0};
    if (stat(storageDir, &st) == -1)
    {
        if (mkdir(storageDir, 0755) != 0)
        {
            perror("Error: Cannot create storage directory");
            exit(1);
        }
        printf("Created storage directory: %s\n", storageDir);
    }

    // Step 1: Construct socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("Error: Cannot create socket");
        exit(1);
    }

    // Step 2: Bind
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

    // Step 3: Listen
    if (listen(listenfd, BACKLOG) < 0)
    {
        perror("Error: Cannot listen");
        close(listenfd);
        exit(1);
    }

    printf("Server listening on port %d...\n", SERV_PORT);

    // Step 4: Accept connections
    int clientAddrLen = sizeof(clientAddr);
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen);
        if (connfd < 0)
        {
            perror("Error: accept()");
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Send welcome message
        char *welcome = "+OK Welcome to file server";
        send(connfd, welcome, strlen(welcome), 0);

        // Loop to handle multiple file uploads from the same client
        while (1)
        {
            // Receive UPLD command
            int rcvBytes = recv(connfd, buff, BUFF_SIZE, 0);
            if (rcvBytes <= 0)
            {
                printf("Client disconnected or error receiving command\n");
                break;
            }
            buff[rcvBytes] = '\0';
            printf("Received: %s\n", buff);

            // Parse UPLD command
            char filename[256];
            long filesize;
            if (sscanf(buff, "UPLD %s %ld", filename, &filesize) != 2)
            {
                printf("Invalid command format.\n");
                break;
            }

            // Send response to client
            char *resp = "+OK Please send file";
            send(connfd, resp, strlen(resp), 0);

            // Create destination file path
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", storageDir, filename);
            FILE *f = fopen(filepath, "wb");
            if (f == NULL)
            {
                perror("Error: Cannot open file to write");
                break;
            }

            printf("Receiving file '%s' (%ld bytes)...\n", filename, filesize);

            // Receive file data
            long received = 0;
            while (received < filesize)
            {
                int bytes = recv(connfd, buff, BUFF_SIZE, 0);
                if (bytes <= 0)
                    break;
                fwrite(buff, 1, bytes, f);
                received += bytes;
            }

            fclose(f);
            printf("File '%s' received (%ld/%ld bytes)\n", filename, received, filesize);

            // Send completion notification
            char *done = "+OK Successful upload";
            send(connfd, done, strlen(done), 0);
        }

        close(connfd);
        printf("Connection with client closed\n\n");
    }

    close(listenfd);
    return 0;
}
