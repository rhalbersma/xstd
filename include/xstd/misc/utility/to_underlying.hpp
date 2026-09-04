//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_UTILITY_TO_UNDERLYING_HPP
#define XSTD_MISC_UTILITY_TO_UNDERLYING_HPP

#include <type_traits> // integral_constant, is_enum_v, underlying_type_t
#include <utility>     // to_underlying

namespace xstd {

// Delegates to the standard's, so one spelling covers a plain enum and a wrapped one alike.
template<class Enum>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(Enum value) noexcept
        -> std::underlying_type_t<Enum>
{
        return std::to_underlying(value);
}

// Preserves compile-time constantness for an integral_constant-wrapped enum.
template<class Enum, Enum N>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept
{
        return std::integral_constant<std::underlying_type_t<Enum>, std::to_underlying(N)>();
}

} // namespace xstd

#endif // XSTD_MISC_UTILITY_TO_UNDERLYING_HPP
