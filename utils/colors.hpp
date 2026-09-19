#ifndef COLORS_HPP
#define COLORS_HPP

#include <string_view>

namespace ansi {

inline constexpr std::string_view Red = "\x1b[31m";
inline constexpr std::string_view Green = "\x1b[32m";
inline constexpr std::string_view Yellow = "\x1b[33m";
inline constexpr std::string_view Blue = "\x1b[34m";
inline constexpr std::string_view Magenta = "\x1b[35m";
inline constexpr std::string_view Cyan = "\x1b[36m";
inline constexpr std::string_view Reset = "\x1b[0m";

}

#endif
