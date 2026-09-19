#include <iostream>
#include <optional>
#include <string>

#include "readline/reader.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

namespace catshell {

std::optional<std::string> read_line() {
    std::cout << ansi::Cyan << '[' << current_directory() << "]$ " << ansi::Reset;
    std::cout.flush();

    std::string line;
    if (!std::getline(std::cin, line)) {
        if (std::cin.eof()) {
            std::cout << ansi::Blue << "[EOF]\n" << ansi::Reset;
        } else {
            std::cout << ansi::Blue << "Error occurred\n" << ansi::Reset;
        }
        return std::nullopt;
    }
    return line;
}

}
