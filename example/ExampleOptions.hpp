/*
 * ScopeTimer - lightweight C++20 scope timing utility
 * Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com> https://xyzzy.tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * In accordance with section 13 of the AGPL, if you modify this program,
 * your modified version must prominently offer all users interacting with it
 * remotely through a computer network an opportunity to receive the source
 * code of your version.
 */

#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace xyzzy::scopetimer::example_options {

inline constexpr unsigned MaxDemoIterations = 32U;
inline constexpr unsigned MaxBenchmarkIterations = 100U;
inline constexpr unsigned MaxBenchmarkThreads = 32U;
inline constexpr std::size_t MaxBenchmarkSinkBytes = 64U * 1024U * 1024U;
inline constexpr std::uint64_t MaxBenchmarkTimerRecords = 10'000'000U;

class OptionError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

inline unsigned parseBoundedUnsigned(
    std::string_view text,
    std::string_view optionName,
    unsigned minimum,
    unsigned maximum
) {
    if (text.empty()) {
        throw OptionError(std::string(optionName) + " must not be empty");
    }
    unsigned value = 0U;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    if (const auto result = std::from_chars(begin, end, value, 10);
        result.ec != std::errc{} || result.ptr != end) {
        throw OptionError(
            std::string(optionName) + " must be a whole decimal integer; got '" +
            std::string(text) + "'"
        );
    }
    if (value < minimum || value > maximum) {
        throw OptionError(
            std::string(optionName) + " must be between " + std::to_string(minimum) +
            " and " + std::to_string(maximum) + "; got '" + std::string(text) + "'"
        );
    }
    return value;
}

inline std::size_t parseBoundedSize(
    std::string_view text,
    std::string_view optionName,
    std::size_t minimum,
    std::size_t maximum
) {
    if (text.empty()) {
        throw OptionError(std::string(optionName) + " must not be empty");
    }
    std::size_t value = 0U;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    if (const auto result = std::from_chars(begin, end, value, 10);
        result.ec != std::errc{} || result.ptr != end) {
        throw OptionError(
            std::string(optionName) + " must be a whole decimal integer; got '" +
            std::string(text) + "'"
        );
    }
    if (value < minimum || value > maximum) {
        throw OptionError(
            std::string(optionName) + " must be between " + std::to_string(minimum) +
            " and " + std::to_string(maximum) + "; got '" + std::string(text) + "'"
        );
    }
    return value;
}

inline std::string uppercaseAscii(std::string_view text) {
    std::string normalized;
    normalized.reserve(text.size());
    for (const char value : text) {
        if (value >= 'a' && value <= 'z') {
            normalized.push_back(static_cast<char>(value - ('a' - 'A')));
        } else {
            normalized.push_back(value);
        }
    }
    return normalized;
}

} // namespace xyzzy::scopetimer::example_options
