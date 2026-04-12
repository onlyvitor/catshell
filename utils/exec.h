#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../commands/commands.h"

typedef struct s_builtin {
    char *name;
    int (*func)(char **args);
} builtin_t;

void exec_command(char **args);

#endif
