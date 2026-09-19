#include <cstring>
#include <filesystem>

#include "utils/utils.hpp"

std::string current_directory() {
    return std::filesystem::current_path().string();
}

extern "C" char *get_current_directory(char *buffer, size_t size) {
    if (buffer == nullptr || size == 0) {
        return nullptr;
    }
    try {
        std::string cwd = current_directory();
        if (cwd.size() >= size) {
            return nullptr;
        }
        std::memcpy(buffer, cwd.c_str(), cwd.size() + 1);
        return buffer;
    } catch (...) {
        return nullptr;
    }
}
