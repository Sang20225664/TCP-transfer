#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

/**
 * @brief Write a log entry to the log file
 * @param mssv Student ID used in log filename
 * @param clientIP Client IP address
 * @param clientPort Client port number
 * @param command Command issued by client (empty for welcome message)
 * @param response Server response message
 * - description
 * 1. Open log file named "log_<mssv>.txt" in append mode
 * 2. Get current timestamp in "DD/MM/YYYY HH:MM:SS" format
 * 3. Write log entry in format:
 *    - For upload commands: [time]$IP:Port$command$response
 *    - For welcome message: [time]$IP:Port$response
 * 4. Close the log file
 */

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
