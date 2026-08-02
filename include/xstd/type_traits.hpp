//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

#include <compare>     // strong_ordering (empty_type's defaulted <=>)
#include <type_traits> // bool_constant, conditional_t, integral_constant, is_same_v, remove_cvref_t

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

        // constrained so that this catch-all never hijacks copy or move
        // construction from the (trivial) special member functions
        // clang-format off
        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept {}
        // clang-format on

        // a hidden friend, so it is found by argument-dependent lookup only;
        // still implicitly declares the defaulted operator== that lets an
        // enclosing class default its own comparisons over this member
        [[nodiscard]] friend constexpr auto operator<=>(empty_type const&, empty_type const&) noexcept
                -> std::strong_ordering = default;
};

// Tag names the member, not the type it stands in for: two
// [[no_unique_address]] subobjects of the same empty type must have
// distinct addresses, so two absent members sharing a tag stop overlapping
// and grow the class. Tagging by Type instead would make that collision a
// silent function of the member types, so Tag is required rather than
// defaulted. An elaborated-type-specifier declares one in place, without a
// separate declaration per member:
//
//      conditional_data_member_t<Condition, Type, struct variable_tag> m_variable [[no_unique_address]];
//
template<bool Condition, class Type, class Tag>
using conditional_data_member_t = std::conditional_t<Condition, Type, empty_type<Tag>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_HPP
