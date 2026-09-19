#include <iostream>

#include "commands/commands.h"

extern char **environ;

extern "C" int env_func(char **args) {
    (void)args;
    if (environ == nullptr) {
        return 1;
    }
    for (int i = 0; environ[i] != nullptr; i++) {
        std::cout << environ[i] << '\n';
    }
    return 0;
}
