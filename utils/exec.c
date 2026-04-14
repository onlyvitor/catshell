#include "exec.h"


builtin_t g_builtin[] = {
    {.name="echo", .func=echo_func},//echo
    //{builtin_name="env", func=env_func},
    {.name="exit", .func= exit_func},//exit
    {.name="", .func= NULL}
};

void exec_command(char **args) {
    int i = 0;
    const char *curr;
    while((curr = g_builtin[i].name)) {
        if (strcmp(args[0], curr) == 0) {
            g_builtin[i].func(args);
            return;
        }
        i++;
    }

    //catshell_launch(args[0]);
}
