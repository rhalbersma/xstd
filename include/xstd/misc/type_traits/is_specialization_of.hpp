//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
#define XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP

#include <type_traits> // bool_constant

namespace xstd {

// Whether T is a specialization of a type-parameter-only class template.
template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_v = false;

template<template<class...> class Primary, class... Args>
inline constexpr auto is_specialization_of_v<Primary<Args...>, Primary> = true;

template<class T, template<class...> class Primary>
using is_specialization_of = std::bool_constant<is_specialization_of_v<T, Primary>>;

} // namespace xstd

#endif // XSTD_MISC_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
