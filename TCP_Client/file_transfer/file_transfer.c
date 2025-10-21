#include "file_transfer.h"
#include "../validation/validation.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFF_SIZE 16384 // 16KB

/**
 * @brief Receive a message from the socket
 * @param sockfd Socket file descriptor
 * @param buff Buffer to store the received message
 * @param size Size of the buffer
 * @return Number of bytes received, -1 on erro
 */
int recv_message(int sockfd, char *buff, size_t size)
{
    int ret = recv(sockfd, buff, size, 0);
    if (ret <= 0)
        return -1;
    buff[ret] = '\0';
    return ret;
}

/**
 * @brief Send a file to the server
 * @param sockfd Socket file descriptor
 * @param filepath Path of the file to send
 * @return 0 on success, -1 on error
 * - description
 * 1. Send "UPLD <filename> <filesize>" command to server
 * 2. Wait for server to respond with "+OK" to proceed
 * 3. Send the file content in chunks
 * 4. Wait for final response from server
 */
int send_file(int sockfd, const char *filepath)
{
    char buff[BUFF_SIZE + 1];
    const char *filename = get_filename(filepath);
    long filesize = get_file_size(filepath);

    snprintf(buff, sizeof(buff), "UPLD %s %ld", filename, filesize);
    send(sockfd, buff, strlen(buff), 0);

    // Wait for server ready
    int ret = recv_message(sockfd, buff, sizeof(buff));
    if (ret <= 0 || strstr(buff, "+OK") == NULL)
    {
        printf("Server not ready for file transfer.\n");
        return -1;
    }

    // Send file content
    FILE *f = fopen(filepath, "rb");
    if (!f)
    {
        perror("Error opening file");
        return -1;
    }

    printf("Uploading '%s' (%ld bytes)...\n", filename, filesize);
    long sent = 0;
    while (!feof(f))
    {
        size_t bytesRead = fread(buff, 1, BUFF_SIZE, f);
        if (bytesRead > 0)
        {
            send(sockfd, buff, bytesRead, 0);
            sent += bytesRead;
        }
    }
    fclose(f);

    printf("Sent %ld bytes.\n", sent);

    // Final response
    ret = recv_message(sockfd, buff, sizeof(buff));
    if (ret > 0)
        printf("Server final response: %s\n", buff);
    return 0;
}
