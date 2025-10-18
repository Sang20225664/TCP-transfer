#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define BUFF_SIZE 1024

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

    // Step 3: Connect to server
    if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Error: Cannot connect to server");
        close(clientfd);
        exit(1);
    }

    printf("✅ Connected to server %s:%d successfully!\n", serverIP, SERV_PORT);

    // Step 4: Receive welcome message
    int ret = recv(clientfd, buff, BUFF_SIZE, 0);
    if (ret < 0)
    {
        perror("Error: Cannot receive welcome message");
        close(clientfd);
        exit(1);
    }
    buff[ret] = '\0';
    printf("Message from server: %s\n", buff);

    // Step 5: Send UPLD command
    char filepath[256];
    printf("Enter file path to upload: ");
    fgets(filepath, sizeof(filepath), stdin);
    filepath[strcspn(filepath, "\n")] = '\0'; // Remove newline character

    if (strlen(filepath) == 0)
    {
        printf("No file entered. Exiting.\n");
        close(clientfd);
        return 0;
    }

    // Get filename (remove path)
    char *filename = strrchr(filepath, '/');
    if (filename)
        filename++;
    else
        filename = filepath;

    // Get file size
    struct stat st;
    if (stat(filepath, &st) != 0)
    {
        perror("Error: Cannot get file size");
        close(clientfd);
        exit(1);
    }
    long filesize = st.st_size;

    // Create UPLD message
    snprintf(buff, sizeof(buff), "UPLD %s %ld\r\n", filename, filesize);
    printf("Sending command: %s", buff);

    ret = send(clientfd, buff, strlen(buff), 0);
    if (ret < 0)
    {
        perror("Error: Cannot send command");
        close(clientfd);
        exit(1);
    }

    // Step 6: Receive response from server
    ret = recv(clientfd, buff, BUFF_SIZE, 0);
    if (ret < 0)
    {
        perror("Error: Cannot receive server response");
        close(clientfd);
        exit(1);
    }

    buff[ret] = '\0';
    printf("Response from server: %s\n", buff);

    // Step 7: Close connection
    close(clientfd);
    printf("Connection closed.\n");
    return 0;
}
