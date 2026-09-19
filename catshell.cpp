#include <cstdlib>
#include <iostream>
#include <string>

#include "readline/cat_read_line.h"
#include "readline/parser.hpp"
#include "utils/arts/banner.h"
#include "utils/exec.hpp"

int main() {
    print_banner();
    std::cout.flush();

    while (true) {
        char *line = cat_read_line();
        if (line == nullptr) {
            break;
        }
        std::string input(line);
        free(line);

        Pipeline pipeline = parse_input(input);
        exec_pipeline(pipeline);
    }
    return EXIT_SUCCESS;
}
