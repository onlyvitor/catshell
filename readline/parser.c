#include "parser.h"

char** parse_input(char* input) {
    size_t buffer_size = sizeof(char*);
    char** tokens = malloc(buffer_size);
    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

}
