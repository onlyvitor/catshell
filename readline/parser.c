#define _POSIX_C_SOURCE 200809L
#include "parser.h"

static char** parse_command(char* input) {
    char** tokens = malloc((MAX_ARGS + 1) * sizeof(char*));
    size_t token_position = 0;

    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char* saveptr = NULL;
    char* token = strtok_r(input, " ", &saveptr);
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
        token = strtok_r(NULL, " ", &saveptr);
    }

    tokens[token_position] = NULL;
    return tokens;
}

pipeline_t* parse_input(char* input) {
    pipeline_t* pipeline = malloc(sizeof(pipeline_t));
    if (pipeline == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pipeline->commands = malloc(MAX_PIPES * sizeof(char**));
    if (pipeline->commands == NULL) {
        perror("malloc");
        free(pipeline);
        exit(EXIT_FAILURE);
    }

    pipeline->num_commands = 0;

    char* input_copy = strdup(input);
    if (input_copy == NULL) {
        perror("strdup");
        free(pipeline->commands);
        free(pipeline);
        exit(EXIT_FAILURE);
    }

    char* saveptr_pipe = NULL;
    char* segment = strtok_r(input_copy, "|", &saveptr_pipe);
    while (segment != NULL && pipeline->num_commands < MAX_PIPES) {
        while (*segment == ' ') segment++;
        size_t seg_len = strlen(segment);
        if (seg_len > 0) {
            char* end = segment + seg_len - 1;
            while (end > segment && *end == ' ') end--;
            *(end + 1) = '\0';
        }

        if (strlen(segment) > 0) {
            pipeline->commands[pipeline->num_commands] = parse_command(segment);
            pipeline->num_commands++;
        }

        segment = strtok_r(NULL, "|", &saveptr_pipe);
    }

    free(input_copy);
    return pipeline;
}

void free_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL) return;

    for (int i = 0; i < pipeline->num_commands; i++) {
        if (pipeline->commands[i] != NULL) {
            for (int j = 0; pipeline->commands[i][j] != NULL; j++) {
                free(pipeline->commands[i][j]);
            }
            free(pipeline->commands[i]);
        }
    }
    free(pipeline->commands);
    free(pipeline);
}