#include "catshell.h"

#define MAGIC_NUMBER 0xCE77

int main(int ac, char **av) {
    char *line;
    char **args;

    //REPL (read, evaluate, print, loop)
    while(MAGIC_NUMBER){
        // read
        printf("%s", line);
        line = cat_read_line();
        if (line == NULL) {
            break;  // EOF (CTRL+D) pressed
        }
    }
    return EXIT_SUCCESS;
}
