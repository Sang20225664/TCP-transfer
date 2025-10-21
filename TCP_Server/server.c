#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "logger/logger.h"
#include "message/message.h"
#include "receiver/file_receiver.h"

#define BACKLOG 10
#define BUFF_SIZE 1024
#define mssv "20225664"

/**
 * @brief Main function for TCP server
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 * - description
 * 1. Parse command line arguments for port number and storage directory
 * 2. Create socket, bind, and listen for incoming connections
 * 3. Accept client connections and handle file uploads
 * 4. Log all activities using writeLog function
 */

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
        send_message(connfd, welcome);

        writeLog(mssv, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),
                 "", welcome);

        // Receive multiple files from the same client
        while (1)
        {
            memset(buff, 0, sizeof(buff));
            if (recv_message(connfd, buff, BUFF_SIZE) <= 0)
            {
                printf("Client disconnected.\n");
                break;
            }

            printf("Received: %s\n", buff);

            // Parse command
            char filename[256];
            long filesize;
            sscanf(buff, "UPLD %s %ld", filename, &filesize);

            // Send server ready response
            char *resp = "+OK Please send file";
            send_message(connfd, resp);

            // Receive file
            int status = receive_file(connfd, storageDir, filename, filesize);
            if (status == 0)
            {
                char *done = "+OK Successful upload";
                send_message(connfd, done);
                writeLog(mssv, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff, done);
            }
            else
            {
                char *fail = "-ERR File receive failed";
                send_message(connfd, fail);
                writeLog(mssv, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff, fail);
            }
        }

        close(connfd);
        printf("Connection closed.\n\n");
    }

    close(listenfd);
    return 0;
}
