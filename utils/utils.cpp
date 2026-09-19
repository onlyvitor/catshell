#include <filesystem>

#include "utils/utils.hpp"

std::string current_directory() {
    return std::filesystem::current_path().string();
}
