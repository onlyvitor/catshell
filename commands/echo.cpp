#include <iostream>

#include "commands/commands.hpp"

int builtin_echo(const Args &args) {
    for (size_t i = 1; i < args.size(); i++) {
        std::cout << args[i] << ' ';
    }
    std::cout << '\n';
    return 0;
}
