#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // disjunction, enable_if_t, integral_constant, is_enum_v, is_same, underlying_type_t

namespace xstd {

template<class Enumeration, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_underlying_type(Enumeration e) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(e);
}

template<class Enumeration, Enumeration N, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_underlying_type(std::integral_constant<Enumeration, N>) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(N);
}

template<class T, class... Args>
using is_any_of = std::disjunction<std::is_same<T, Args>...>;

template<class T, class... Args>
constexpr auto is_any_of_v = is_any_of<T, Args...>::value;

}       // namespace xstd
