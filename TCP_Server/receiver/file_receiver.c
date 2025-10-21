#include "file_receiver.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

/**
 * @brief Receive a file from the client
 * @param connfd Connection file descriptor
 * @param storageDir Directory to store the received file
 * @param filename Name of the file to receive
 * @param filesize Size of the file to receive
 * @return 0 on success, -1 on error
 * - description
 * 1. Construct full file path by combining storageDir and filename
 * 2. Open file in binary write mode
 * 3. Loop to receive data in chunks until filesize is reached
 * 4. Write received data to file
 * 5. Close the file
 */

int receive_file(int connfd, const char *storageDir, const char *filename, long filesize)
{
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", storageDir, filename);

    FILE *f = fopen(filepath, "wb");
    if (!f)
    {
        perror("Error: Cannot open file to write");
        return -1;
    }

    char buff[BUFF_SIZE];
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

    return (received == filesize) ? 0 : -1;
}
