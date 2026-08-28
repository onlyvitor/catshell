#include "parser.h"

char** parse_input(char* input) {
    char** tokens = malloc((MAX_ARGS + 1) * sizeof(char*));
    size_t token_position = 0;

    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(input, " ");
    while (token != NULL) {
        if (token_position >= MAX_ARGS) {
            fprintf(stderr, "parse_input: too many arguments (max %d)\n", MAX_ARGS);
            break;
        }

        tokens[token_position] = malloc(strlen(token) + 1);
        if (tokens[token_position] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        strcpy(tokens[token_position], token);
        token_position++;
        token = strtok(NULL, " ");
    }

    tokens[token_position] = NULL;
    return tokens;
}
