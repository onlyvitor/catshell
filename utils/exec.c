#include "exec.h"


builtin_t g_builtin[] = {
    {.name="echo", .func=echo_func},//echo
    {.name="env", .func=env_func},//env
    {.name="exit", .func=exit_func},//exit
    {.name="", .func=NULL}
};

void exec_command(char **args) {
    if (args[0] == NULL) {
        return;  // empty line, nothing to do
    }

    for (size_t i = 0; g_builtin[i].name[0] != '\0'; i++) {
        if (strcmp(args[0], g_builtin[i].name) == 0) {
            g_builtin[i].func(args);
            return;
        }
    }

    printf("%s: command not found\n", args[0]);
    cat_launch(args);
}

void cat_launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        // parent
        wait(NULL);
    }
}
