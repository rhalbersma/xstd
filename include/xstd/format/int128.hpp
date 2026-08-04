//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_INT128_HPP
#define XSTD_FORMAT_INT128_HPP

#include <xstd/config/int128.hpp> // int128_t, uint128_t
#include <algorithm>              // ranges::reverse
#include <concepts>               // signed_integral
#include <string>                 // string, to_string

namespace xstd::detail {

template<class T>
        requires std::signed_integral<T>
[[nodiscard]] auto to_decimal(T value)
        -> std::string
{
        return std::to_string(static_cast<long long>(value));
}

[[nodiscard]] inline auto to_decimal(uint128_t value)
        -> std::string
{
        if (value == uint128_t{0}) {
                return "0";
        }

        auto result = std::string{};
        while (value != uint128_t{0}) {
                auto const digit = static_cast<unsigned>(value % uint128_t{10});
                result.push_back(static_cast<char>('0' + digit));
                value /= uint128_t{10};
        }
        std::ranges::reverse(result);
        return result;
}

[[nodiscard]] inline auto to_decimal(int128_t value)
        -> std::string
{
        if (value >= int128_t{0}) {
                return to_decimal(static_cast<uint128_t>(value));
        }

        auto result = std::string{"-"};
        result += to_decimal(static_cast<uint128_t>(-(value + int128_t{1})) + uint128_t{1});
        return result;
}

} // namespace xstd::detail

#endif // XSTD_FORMAT_INT128_HPP
