#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define BUFF_SIZE 16384 // 16KB block size

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

    // Step 2: Server address
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, serverIP, &servAddr.sin_addr) <= 0)
    {
        perror("Error: Invalid address");
        close(clientfd);
        exit(1);
    }

    // Step 3: Connect
    if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Error: Cannot connect to server");
        close(clientfd);
        exit(1);
    }
    printf("Connected to server %s:%d successfully!\n", serverIP, SERV_PORT);

    // Step 4: Receive welcome
    int ret = recv(clientfd, buff, BUFF_SIZE, 0);
    if (ret <= 0)
    {
        perror("Error: Cannot receive welcome message");
        close(clientfd);
        exit(1);
    }
    buff[ret] = '\0';
    printf("Message from server: %s\n", buff);

    // Step 5: Loop for multiple file uploads
    while (1)
    {
        // Input file path
        char filepath[256];
        printf("Enter file path to upload (empty to exit): ");
        fgets(filepath, sizeof(filepath), stdin);
        filepath[strcspn(filepath, "\n")] = '\0';

        // Check if user wants to exit
        if (strlen(filepath) == 0)
        {
            printf("Exiting.\n");
            break;
        }

        // Get filename
        char *filename = strrchr(filepath, '/');
        filename = filename ? filename + 1 : filepath;

        // Get filesize
        struct stat st;
        if (stat(filepath, &st) != 0)
        {
            perror("Error: Cannot get file size");
            continue; // Continue to next iteration instead of exiting
        }
        long filesize = st.st_size;

        // Send upload command
        snprintf(buff, sizeof(buff), "UPLD %s %ld", filename, filesize);
        send(clientfd, buff, strlen(buff), 0);

        // Wait for server to allow file transfer
        ret = recv(clientfd, buff, BUFF_SIZE, 0);
        if (ret <= 0)
        {
            perror("Error: No response from server");
            break;
        }
        buff[ret] = '\0';
        printf("Server response: %s\n", buff);

        // Send file content
        FILE *f = fopen(filepath, "rb");
        if (f == NULL)
        {
            perror("Error: Cannot open file");
            continue; // Continue to next iteration instead of exiting
        }

        printf("Uploading '%s' (%ld bytes)...\n", filename, filesize);

        long sentBytes = 0;
        while (!feof(f))
        {
            size_t bytesRead = fread(buff, 1, BUFF_SIZE, f);
            if (bytesRead > 0)
            {
                size_t bytesSent = send(clientfd, buff, bytesRead, 0);
                if (bytesSent < 0)
                {
                    perror("Error: send()");
                    fclose(f);
                    break;
                }
                sentBytes += bytesSent;
            }
        }
        fclose(f);
        printf("Sent %ld bytes to server.\n", sentBytes);

        // Wait for success confirmation
        ret = recv(clientfd, buff, BUFF_SIZE, 0);
        if (ret > 0)
        {
            buff[ret] = '\0';
            printf("Server final response: %s\n", buff);
        }
        else
        {
            perror("Error: No final response");
        }

        printf("\n"); // Add spacing between uploads
    }

    close(clientfd);
    printf("Connection closed.\n");
    return 0;
}
