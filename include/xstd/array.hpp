#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <array>        // array
#include <type_traits>

namespace xstd {

template<class L>
struct array_from_types;

template<template<class...> class L, class... T>
struct array_from_types<L<T...>>
{
        constexpr auto operator()(auto fun) const noexcept((noexcept(fun(T())) && ...))
        {
                return std::array{fun(T())...};
        }
};

template<class Tag>
struct tagged_empty
{
        tagged_empty() = default;

        template<class... Args>
        constexpr explicit tagged_empty(Args&&...) noexcept {}
};

template<bool Condition, class Base>
using or_empty = std::conditional_t<Condition, Base, tagged_empty<Base>>;

}       // namespace xstd
