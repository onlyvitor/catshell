#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "readline/parser.hpp"

Pipeline parse_input(const std::string &input) {
    Pipeline pipeline;
    std::istringstream stream(input);
    std::string segment;
    while (std::getline(stream, segment, '|')) {
        std::istringstream tokens(segment);
        Command command;
        std::string token;
        while (tokens >> token) {
            command.args.push_back(token);
        }
        if (!command.args.empty()) {
            pipeline.commands.push_back(std::move(command));
            if (pipeline.commands.size() >= MaxPipes) {
                std::cerr << "parse_input: too many pipelines (max " << MaxPipes << ")\n";
                break;
            }
        }
    }
    return pipeline;
}
