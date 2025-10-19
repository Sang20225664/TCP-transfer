#include "validation.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

int validate_file(const char *filepath)
{
    struct stat st;
    return (stat(filepath, &st) == 0 && S_ISREG(st.st_mode));
}

long get_file_size(const char *filepath)
{
    struct stat st;
    if (stat(filepath, &st) != 0)
        return -1;
    return st.st_size;
}

const char *get_filename(const char *filepath)
{
    const char *slash = strrchr(filepath, '/');
    return (slash) ? slash + 1 : filepath;
}
