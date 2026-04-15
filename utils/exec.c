#include "exec.h"


builtin_t g_builtin[] = {
    {.name="echo", .func=echo_func},//echo
    {.name="env", .func=env_func},//env
    {.name="exit", .func=exit_func},//exit
    {.name="", .func=NULL}
};

void exec_command(char **args) {
    int i = 0;
    const char *curr;
    while((curr = g_builtin[i].name)) {
        if (strcmp(args[0], curr) == 0) {
            g_builtin[i].func(args);
            return;
        } else {
            printf("%s: command not found\n", args[0]);
        }
        i++;
    }

    cat_launch(args);
}

void cat_launch(char **args) {
    if (fork() == cat_child) {
        execvp(args[0], args);
        _exit(1);
    }else{
        wait(NULL);
    }
}
