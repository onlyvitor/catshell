#include <iostream>

#include "commands/commands.hpp"

extern char **environ;

int builtin_env(const Args &args) {
    (void)args;
    if (environ == nullptr) {
        return 1;
    }
    for (int i = 0; environ[i] != nullptr; i++) {
        std::cout << environ[i] << '\n';
    }
    return 0;
}
