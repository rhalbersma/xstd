#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // conditional_t, is_same_v

namespace xstd {

template<class T, class... Args>
inline constexpr auto any_of = (std::is_same_v<T, Args> || ...);

template<class Tag>
struct empty
{
        empty() = default;

        template<class... Args>
        constexpr explicit empty(Args&&...) noexcept {}
};

template<bool Condition, class Base>
using or_empty = std::conditional_t<Condition, Base, empty<Base>>;

}       // namespace xstd
