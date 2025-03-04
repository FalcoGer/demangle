#include "Demangle.h"
#include <cstdint>
#include <print>
#include <string>
#include <type_traits>

auto main() -> int
{
    using Demangler::demangle;

    auto name = demangle<std::common_type_t<std::int16_t, std::int32_t>>();
    std::println("Commontype<int16, int32> = {}", name);

    name = demangle<int&>();
    std::println("int& = {}", name);

    name = demangle<int*>();
    std::println("int* = {}", name);

    name = demangle<int* const>();
    std::println("int* const = {}", name);

    name = demangle<const int&>();
    std::println("const int& = {}", name);

    name = demangle<const int* const>();
    std::println("const int* const = {}", name);

    name = demangle<std::string>();
    std::println("std::string = {}", name);

    return 0;
}
