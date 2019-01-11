#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <array>        // array

namespace xstd {

template<class L>
struct array_from_types;

template<template<class...> class L, class... T>
struct array_from_types<L<T...>>
{
        template<class UnaryFunction>
        constexpr auto operator()(UnaryFunction fun) const
        {
                return std::array{fun(T{})...};
        }
};

}       // namespace xstd
