#ifndef EXEC_H
#define EXEC_H

#define cat_child 0

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../commands/commands.h"

typedef struct s_builtin {
    char *name;
    int (*func)(char **args);
} builtin_t;

void exec_command(char **args);
void cat_launch(char **args);
#endif
