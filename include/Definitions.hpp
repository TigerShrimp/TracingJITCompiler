#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#include <fmt/core.h>

#include <iostream>
// #define DEBUG_PRINT_ON
#ifdef DEBUG_PRINT_ON

// TODO: do this with std::format instead as soon as clang adds support for it.
#define DEBUG_PRINT(...) std::cout << fmt::format(__VA_ARGS__);
#else
#define DEBUG_PRINT(...) fmt::format(__VA_ARGS__);
#endif
#endif  // DEFINITIONS_HPP