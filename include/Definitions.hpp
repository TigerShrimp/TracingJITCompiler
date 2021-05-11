#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#include <fmt/core.h>

#include <iostream>
// #define TRACE_INFO
// #define INTERPRETING_ONLY
// #define DEBUG_PRINT_ON
#ifdef DEBUG_PRINT_ON

// TODO: do this with std::format instead as soon as clang adds support for it.
#define DEBUG_PRINT(...) std::cout << fmt::format(__VA_ARGS__);
#else
// Here we trick the compiler to still expand the arguments of the macro. This
// because it helps us deal with GDB a little bit better
#define DEBUG_PRINT(...) fmt::format(__VA_ARGS__);
#endif
#endif  // DEFINITIONS_HPP
