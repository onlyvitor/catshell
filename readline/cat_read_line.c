#include "cat_read_line.h"

char *cat_read_line(void) {
    char *buf;
    size_t buf_size;
    int position;

    char **args;

    printf("~$ ");

    buf = NULL;
    if (getline(&buf, &buf_size, stdin) == -1) {
        buf = NULL;

        if (feof(stdin)) {
            printf(ANSI_COLOR_BLUE"[EOF]"RESET);
        }
        else{
            printf(ANSI_COLOR_BLUE" a Error ocurred\n"RESET);
        }
    }
    return buf;
}
