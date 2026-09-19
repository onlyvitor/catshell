#include <cstdlib>

#include "commands/commands.h"

extern "C" int exit_func(char **args) {
    (void)args;
    std::exit(0);
}
