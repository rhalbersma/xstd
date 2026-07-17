//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

#include <compare>      // strong_ordering (tagged_empty's defaulted <=>)
#include <type_traits>  // bool_constant, conditional_t, integral_constant, is_same_v, remove_cvref_t

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

        // constrained so that this catch-all never hijacks copy or move
        // construction from the (trivial) special member functions
        template<class... Args>
                requires (!(std::is_same_v<std::remove_cvref_t<Args>, tagged_empty> || ...))
        constexpr explicit tagged_empty(Args&&...) noexcept {}

        auto operator<=>(tagged_empty const&) const = default;
};

template<bool Condition, class Base>
using optional_type = std::conditional_t<Condition, Base, tagged_empty<Base>>;

}       // namespace xstd

#endif  // XSTD_TYPE_TRAITS_HPP
