#include "catshell.h"

#define MAGIC_NUMBER 0xCE77

int main(int ac, char **av) {
    char *line;
    char **args;

    // Print banner on startup
    print_banner();

    //REPL (read, evaluate, print, loop)
    while(MAGIC_NUMBER){
        // read
        line = cat_read_line();
        if (line == NULL) {
            break;  // EOF (CTRL+D) pressed
        }
        printf("%s", line);
    }
    return EXIT_SUCCESS;
}
