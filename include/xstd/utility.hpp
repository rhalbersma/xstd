#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // integral_constant, is_enum_v, underlying_type_t

namespace xstd {

template<class Enumeration>
        requires std::is_enum_v<Enumeration>
constexpr auto to_underlying(Enumeration e) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(e);
}

template<class Enumeration, Enumeration N>
        requires std::is_enum_v<Enumeration>
constexpr auto to_underlying(std::integral_constant<Enumeration, N>) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(N);
}

}       // namespace xstd
