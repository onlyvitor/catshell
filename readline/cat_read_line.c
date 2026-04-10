#include "cat_read_line.h"

char *cat_read_line(void) {
    char *buf;
    size_t buf_size;
    char cwd[BUFSIZ];

    buf = NULL;
    char *current_dir = get_current_directory(cwd, sizeof(cwd));

    printf(ANSI_COLOR_RED"[%s]$ "RESET, current_dir);


    if (getline(&buf, &buf_size, stdin) == -1) {
        buf = NULL;

        if (feof(stdin)) {
            printf(ANSI_COLOR_BLUE"[EOF]\n"RESET);
        }
        else{
            printf(ANSI_COLOR_BLUE" a Error ocurred\n"RESET);
        }
    }
    return buf;
}
