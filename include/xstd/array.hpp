//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_ARRAY_HPP
#define XSTD_ARRAY_HPP

#include <array>        // array
#include <type_traits>  // type_identity

namespace xstd {

template<class L>
struct array_from_types;

template<template<class...> class L, class... T>
struct array_from_types<L<T...>>
{
        // constrained to non-empty type lists: with no elements there is no
        // type for std::array's class template argument deduction to deduce
        [[nodiscard]] constexpr auto operator()(auto fun) const noexcept((noexcept(fun(std::type_identity<T>())) && ...))
                requires (sizeof...(T) > 0)
        {
                return std::array{fun(std::type_identity<T>())...};
        }
};

}       // namespace xstd

#endif  // XSTD_ARRAY_HPP
