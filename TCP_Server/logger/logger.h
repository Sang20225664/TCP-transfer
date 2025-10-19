#ifndef LOGGER_H
#define LOGGER_H

void writeLog(const char *mssv, const char *clientIP, int clientPort,
              const char *command, const char *response);

#endif
