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

        // Gửi chào mừng
        char *welcome = "+OK Welcome to file server";
        send(connfd, welcome, strlen(welcome), 0);

        // Nhận thông điệp từ client
        int rcvBytes = recv(connfd, buff, BUFF_SIZE, 0);
        if (rcvBytes <= 0)
        {
            perror("Error: Cannot receive command");
            close(connfd);
            continue;
        }

        buff[rcvBytes] = '\0';
        printf("Received from client [%s:%d]: %s\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);

        // Phản hồi yêu cầu UPLD
        char *resp = "+OK Please send file";
        send(connfd, resp, strlen(resp), 0);
        printf("Sent response to client [%s:%d]\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        close(connfd);
    }

    close(listenfd);
    return 0;
}
