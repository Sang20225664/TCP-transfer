#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <stddef.h>

int send_file(int sockfd, const char *filepath);
int recv_message(int sockfd, char *buff, size_t size);

#endif
