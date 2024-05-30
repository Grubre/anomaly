#pragma once
#include <expected>
#include <string>
#include <fmt/printf.h>

namespace an {
using Error = std::string;
template <typename T> using Expected = std::expected<T, Error>;

[[noreturn]] inline void throw_error(const Error &error) {
    fmt::println(stderr, "Error: {}", error);
    std::exit(1);
}

template <typename T> auto unwrap(const Expected<T> &exp) -> T {
    if (!exp.has_value()) {
        throw_error(exp.error());
    }

    return exp.value();
}

} // namespace an
