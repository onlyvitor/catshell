#include <cstdlib>

#include "commands/commands.hpp"

int builtin_exit(const Args &args) {
    (void)args;
    std::exit(0);
}
