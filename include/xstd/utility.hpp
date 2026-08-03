//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_UTILITY_HPP
#define XSTD_UTILITY_HPP

#include <type_traits> // integral_constant, is_enum_v, underlying_type_t
#include <utility>     // to_underlying

namespace xstd {

// std::to_underlying (P1682R3) only takes a plain enum value; this overload
// preserves compile-time-constant-ness for an integral_constant-wrapped enum.
// The return type is deduced rather than spelled out for the same reason
// xstd::uabs's is: it would mention std::underlying_type_t<Enum>, and Clang
// before 21 (no CWG2369) substitutes the return type before checking the
// constraint, turning to_underlying(non_enum) in a requires-expression from
// false into a hard error.
template<class Enum, Enum N>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept
{
        return std::integral_constant<std::underlying_type_t<Enum>, std::to_underlying(N)>();
}

} // namespace xstd

#endif // XSTD_UTILITY_HPP
