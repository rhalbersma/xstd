#pragma once

//          Copyright Rein Halbersma 2014-2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // integral_constant, is_enum_v, underlying_type_t

namespace xstd {

template<class Enum>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(Enum e) noexcept
{
        return static_cast<std::underlying_type_t<Enum>>(e);
}

template<class Enum, Enum N>
        requires std::is_enum_v<Enum>
[[nodiscard]] constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept
{
        return std::integral_constant<std::underlying_type_t<Enum>, to_underlying(N)>();
}

}       // namespace xstd
