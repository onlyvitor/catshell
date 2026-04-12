#include "catshell.h"

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

        // parse
        args = parse_input(line);
        printf("%s\n", args[0]);

        printf("%s", line);
    }
    return EXIT_SUCCESS;
}
