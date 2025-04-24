#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <array>        // array
#include <type_traits>  // conditional_t

namespace xstd {

template<class L>
struct array_from_types;

template<template<class...> class L, class... T>
struct array_from_types<L<T...>>
{
        [[nodiscard]] constexpr auto operator()(auto fun) const noexcept((noexcept(fun(T())) && ...))
        {
                return std::array{fun(T())...};
        }
};

}       // namespace xstd
