#include "catshell.h"

#define MAGIC_NUMBER 0xCE77

int main(int ac, char **av) {
    char *line;
    //REPL (read, evaluate, print, loop)
    while(MAGIC_NUMBER){
        // read
        line = cat_read_line();
        if (line == NULL) {
            break;  // EOF (CTRL+D) pressed
        }
        printf(">%s", line);
        pause();

    }
    return EXIT_SUCCESS;
}
