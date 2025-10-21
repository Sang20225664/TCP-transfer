#include "validation.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Validate if the file exists and is readable
 * @param filepath Path of the file to validate
 * @return 1 if valid, 0 otherwise
 */

long get_file_size(const char *filepath)
{
    struct stat st;
    if (stat(filepath, &st) != 0)
        return -1;
    return st.st_size;
}

/**
 * @brief Extract filename from the given file path
 * @param filepath Full path of the file
 * @return Pointer to the filename within the path
 */
const char *get_filename(const char *filepath)
{
    const char *slash = strrchr(filepath, '/');
    return (slash) ? slash + 1 : filepath;
}
