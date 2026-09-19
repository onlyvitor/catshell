#include <iostream>

#include "commands/commands.h"

extern "C" int echo_func(char **args) {
    for (int i = 1; args[i] != nullptr; i++) {
        std::cout << args[i] << ' ';
    }
    std::cout << '\n';
    return 0;
}
