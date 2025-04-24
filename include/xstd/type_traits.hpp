#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <compare>      // partial_ordering
#include <type_traits>  // bool_constant, conditional_t, integral_constant

namespace xstd {

template<class T, class U>
inline constexpr auto is_integral_constant_v = false;

template<class U, U N>
inline constexpr auto is_integral_constant_v<std::integral_constant<U, N>, U> = true;

template<class T, class U>
using is_integral_constant = std::bool_constant<is_integral_constant_v<T, U>>;

template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_v = false;

template<template<class...> class Primary, class... Args>
inline constexpr auto is_specialization_of_v<Primary<Args...>, Primary> = true;

template<class T, template<class...> class Primary>
using is_specialization_of = std::bool_constant<is_specialization_of_v<T, Primary>>;

template<class Tag>
struct tagged_empty
{
        tagged_empty() = default;
        constexpr explicit tagged_empty(auto&&...) noexcept {}
        auto operator<=>(tagged_empty const&) const = default;
};

template<bool Condition, class Base>
using optional_type = std::conditional_t<Condition, Base, tagged_empty<Base>>;

}       // namespace xstd
