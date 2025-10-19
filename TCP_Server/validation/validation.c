#include "validation.h"
#include <stdio.h>
#include <string.h>

int validate_command(const char *cmd)
{
    char keyword[8];
    char filename[256];
    long filesize;

    // Kiểm tra định dạng: UPLD <filename> <filesize>
    if (sscanf(cmd, "%s %s %ld", keyword, filename, &filesize) != 3)
        return 0;

    if (strcmp(keyword, "UPLD") != 0)
        return 0;

    if (filesize <= 0)
        return 0;

    return 1; // hợp lệ
}
