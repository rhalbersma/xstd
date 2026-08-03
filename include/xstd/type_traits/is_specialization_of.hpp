//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
#define XSTD_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP

#include <type_traits>

namespace xstd {

template<class T, template<class...> class Primary>
struct is_specialization_of : std::false_type
{};

template<template<class...> class Primary, class... Args>
struct is_specialization_of<Primary<Args...>, Primary> : std::true_type
{};

template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_v = is_specialization_of<T, Primary>::value;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_SPECIALIZATION_OF_HPP
