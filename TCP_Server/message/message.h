#ifndef MESSAGE_H
#define MESSAGE_H

int send_message(int connfd, const char *msg);
int recv_message(int connfd, char *buffer, int bufsize);

#endif
