#ifndef FILE_RECEIVER_H
#define FILE_RECEIVER_H

int receive_file(int connfd, const char *storageDir, const char *filename, long filesize);

#endif
