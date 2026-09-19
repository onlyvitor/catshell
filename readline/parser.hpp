#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstddef>
#include <string>
#include <vector>

inline constexpr std::size_t MaxPipes = 10;

struct Command {
    std::vector<std::string> args;
};

struct Pipeline {
    std::vector<Command> commands;
};

Pipeline parse_input(const std::string &input);

#endif
