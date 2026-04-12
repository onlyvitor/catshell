#include "parser.h"

char** parse_input(char* input) {
    size_t buffer_size = sizeof(char*);
    char** tokens = malloc(buffer_size + sizeof(char*));
    char* token = NULL;
    size_t token_length = 0;
    size_t token_position = 0;

    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; input[i] ; i++) {
        token = &input[i];

        while (input[i] && input[i] != ' ') {
            token_length++;
            i++;
        }

        tokens[token_position] = malloc(token_length + 1);

        if (tokens[token_position] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        for (size_t j = 0; j < token_length; j++) {
            tokens[token_position][j] = token[j];
        }

        tokens[token_position][token_length] = '\0';
        token_position++;
        token_length = 0;

    }

    tokens[token_position] = NULL;
    return tokens;
}
