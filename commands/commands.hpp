#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>

using Args = std::vector<std::string>;

int builtin_echo(const Args &args);
int builtin_env(const Args &args);
int builtin_exit(const Args &args);

#endif
