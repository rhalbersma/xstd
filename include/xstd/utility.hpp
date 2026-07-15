#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // integral_constant, is_enum_v, underlying_type_t
#include <utility>      // to_underlying

namespace xstd {

// std::to_underlying (P1682R3) only takes a plain enum value; this overload
// preserves compile-time-constant-ness for an integral_constant-wrapped enum.
template<class Enum, Enum N>
[[nodiscard]] constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept
        requires std::is_enum_v<Enum>
{
        return std::integral_constant<std::underlying_type_t<Enum>, std::to_underlying(N)>();
}

}       // namespace xstd
