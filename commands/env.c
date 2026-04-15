#include "commands.h"

int env_func(char **args) {
    extern char **environ;
    if (environ == NULL) {
        return 1;
    }
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}
