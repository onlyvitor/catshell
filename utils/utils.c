#include "utils.h"

char *get_current_directory(char *buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        perror(ANSI_COLOR_RED "get_current_directory: buffer is NULL or size is 0" RESET);
        return NULL;
    }

    if (getcwd(buffer, size) == NULL) {
        perror(ANSI_COLOR_RED "getcwd" RESET);
        return NULL;
    }

    return buffer;
}
