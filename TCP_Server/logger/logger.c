#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

void writeLog(const char *mssv, const char *clientIP, int clientPort,
              const char *command, const char *response)
{
    char filename[64];
    snprintf(filename, sizeof(filename), "log_%s.txt", mssv);

    FILE *f = fopen(filename, "a");
    if (!f)
    {
        perror("Error opening log file");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%d/%m/%Y %H:%M:%S", t);

    // Format based on whether command exists
    if (strlen(command) > 0)
    {
        // For upload commands: [time]$IP:Port$command$response
        fprintf(f, "[%s]$%s:%d$%s$%s\n",
                timeStr, clientIP, clientPort, command, response);
    }
    else
    {
        // For welcome message: [time]$IP:Port$response
        fprintf(f, "[%s]$%s:%d$%s\n",
                timeStr, clientIP, clientPort, response);
    }

    fclose(f);
}
