#include "catshell.h"

int main() {
    char *line;
    pipeline_t *pipeline;

    // Print banner on startup
    print_banner();
    fflush(stdout);

    //REPL (read, evaluate, print, loop)
    while(MAGIC_NUMBER){
        // read
        line = cat_read_line();
        if (line == NULL) {
            break;  // EOF (CTRL+D) pressed
        }

        // parse
        pipeline = parse_input(line);

        //execute
        exec_pipeline(pipeline);

        //liberate memory leak
        free_pipeline(pipeline);
        free(line);
    }
    return EXIT_SUCCESS;
}
