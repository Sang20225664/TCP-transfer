#include "file_receiver.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

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
