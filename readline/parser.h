#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 100
#define MAX_PIPES 10
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_pipeline {
    char ***commands;
    int num_commands;
} pipeline_t;

pipeline_t* parse_input(char* input);
void free_pipeline(pipeline_t* pipeline);

#endif
