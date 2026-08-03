//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

#include <xstd/type_traits/numeric.hpp>            // numeric classification traits
#include <xstd/type_traits/sign_modifications.hpp> // make_signed_like, make_unsigned_like
#include <compare>                                 // strong_ordering (empty_type's defaulted <=>)
#include <type_traits>                             // bool_constant, conditional_t, integral_constant, remove_cvref_t

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
struct empty_type
{
        [[nodiscard]] constexpr empty_type() noexcept = default;

        // lets an enclosing class construct this member similarly as any alternative type
        // constrained so that this catch-all never hijacks copy or move construction
        // clang-format off
        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept {}
        // clang-format on

        // lets an enclosing class default its own comparisons over this member
        [[nodiscard]] friend constexpr auto operator<=>(empty_type, empty_type) noexcept -> std::strong_ordering = default;
};

// Tag keeps multiple absent [[no_unique_address]] members distinct.
template<bool Condition, class Type, class Tag>
using conditional_data_member_t = std::conditional_t<Condition, Type, empty_type<Tag>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_HPP
