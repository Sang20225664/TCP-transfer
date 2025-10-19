#include "message.h"
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int send_message(int connfd, const char *msg)
{
    return send(connfd, msg, strlen(msg), 0);
}

int recv_message(int connfd, char *buffer, int bufsize)
{
    int bytes = recv(connfd, buffer, bufsize - 1, 0);
    if (bytes <= 0)
        return -1;
    buffer[bytes] = '\0';
    return bytes;
}
