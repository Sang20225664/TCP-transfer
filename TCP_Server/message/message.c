#include "message.h"
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief Send a message to the socket
 * @param connfd Connection file descriptor
 * @param msg Message to send
 * @return Number of bytes sent, -1 on error
 * - description
 * Use send() to transmit the message over the socket
 */
int send_message(int connfd, const char *msg)
{
    return send(connfd, msg, strlen(msg), 0);
}
/**
 * @brief Receive a message from the socket
 * @param connfd Connection file descriptor
 * @param buffer Buffer to store the received message
 * @param bufsize Size of the buffer
 * @return Number of bytes received, -1 on error
 * - description
 * Use recv() to read the message from the socket
 */
int recv_message(int connfd, char *buffer, int bufsize)
{
    int bytes = recv(connfd, buffer, bufsize - 1, 0);
    if (bytes <= 0)
        return -1;
    buffer[bytes] = '\0';
    return bytes;
}
