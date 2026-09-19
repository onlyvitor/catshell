#include <cstdlib>
#include <iostream>
#include <string>

#include "readline/reader.hpp"
#include "readline/parser.hpp"
#include "utils/arts/banner.h"
#include "utils/exec.hpp"

int main() {
    print_banner();
    std::cout.flush();

    while (true) {
        std::optional<std::string> line = catshell::read_line();
        if (!line) {
            break;
        }

        Pipeline pipeline = parse_input(*line);
        exec_pipeline(pipeline);
    }
    return EXIT_SUCCESS;
}
