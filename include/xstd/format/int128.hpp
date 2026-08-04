//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_INT128_HPP
#define XSTD_FORMAT_INT128_HPP

#include <xstd/config/int128.hpp> // int128_t, uint128_t
#include <algorithm>              // ranges::reverse
#include <concepts>               // signed_integral
#include <format>                 // format_error, formatter
#include <string>                 // string, to_string

namespace xstd::detail {

inline constexpr auto decimal_base = uint128_t{10};

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
                auto const digit = static_cast<unsigned>(value % decimal_base);
                result.push_back(static_cast<char>('0' + digit));
                value /= decimal_base;
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

template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::uint128_t, char>
{
        constexpr auto parse(std::format_parse_context& ctx)
                -> decltype(ctx.begin())
        {
                auto const it = ctx.begin();
                if (it != ctx.end() and *it != '}') {
                        throw std::format_error{"invalid format specifier for xstd::uint128_t"};
                }
                return it;
        }

        [[nodiscard]] auto format(xstd::uint128_t const value, std::format_context& ctx) const
                -> decltype(ctx.out())
        {
                auto out = ctx.out();
                for (auto const ch : xstd::detail::to_decimal(value)) {
                        *out++ = ch;
                }
                return out;
        }
};

template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::int128_t, char>
{
        constexpr auto parse(std::format_parse_context& ctx)
                -> decltype(ctx.begin())
        {
                auto const it = ctx.begin();
                if (it != ctx.end() and *it != '}') {
                        throw std::format_error{"invalid format specifier for xstd::int128_t"};
                }
                return it;
        }

        [[nodiscard]] auto format(xstd::int128_t const value, std::format_context& ctx) const
                -> decltype(ctx.out())
        {
                auto out = ctx.out();
                for (auto const ch : xstd::detail::to_decimal(value)) {
                        *out++ = ch;
                }
                return out;
        }
};

#endif // XSTD_FORMAT_INT128_HPP
