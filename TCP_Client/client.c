#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "file_transfer/file_transfer.h"

#define BUFF_SIZE 16384 // 16KB block size

/**
 * @brief Main function for TCP client
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 * - description
 * 1. Parse command line arguments for server IP and port
 * 2. Create socket and connect to server
 * 3. Receive welcome message from server
 * 4. Loop to upload files until user decides to exit
 */

int main(int argc, char *argv[])
{
    int clientfd;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in servAddr;

    if (argc != 3)
    {
        printf("Usage: %s <IP_Address> <Port_Number>\n", argv[0]);
        exit(1);
    }

    char *serverIP = argv[1];
    int SERV_PORT = atoi(argv[2]);

    // Step 1: Construct socket
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0)
    {
        perror("Error: Cannot create socket");
        exit(1);
    }

    // Step 2: Connect
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, serverIP, &servAddr.sin_addr);

    if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Error: Cannot connect to server");
        close(clientfd);
        exit(1);
    }

    printf("Connected to server %s:%d successfully!\n", serverIP, SERV_PORT);

    // Step 3: Receive welcome message
    int ret = recv_message(clientfd, buff, BUFF_SIZE);
    if (ret > 0)
        printf("Message from server: %s\n", buff);

    // Step 4: Upload files
    while (1)
    {
        char filepath[256];
        printf("Server is ready to receive file. Enter file path to upload (empty to exit): ");
        fgets(filepath, sizeof(filepath), stdin);
        filepath[strcspn(filepath, "\n")] = '\0';

        if (strlen(filepath) == 0)
            break;

        if (access(filepath, F_OK) != 0)
        {
            printf("Error: Invalid file path.\n");
            continue;
        }

        if (send_file(clientfd, filepath) != 0)
        {
            printf("Error: Failed to upload file.\n");
        }
        printf("\n");
    }

    close(clientfd);
    printf("Connection closed.\n");
    return 0;
}
