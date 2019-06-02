#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // disjunction, enable_if_t, integral_constant, is_enum_v, is_same_v, underlying_type_t

namespace xstd {

template<class Enumeration, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_utype(Enumeration e) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(e);
}

template<class Enumeration, Enumeration N, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_utype(std::integral_constant<Enumeration, N>) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(N);
}

template<class T, class... Args>
inline constexpr auto any_of = (std::is_same_v<T, Args> || ...);

namespace block_adl {

template<class Tag>
struct empty
{
        empty() = default;

        template<class... Args>
        constexpr explicit empty(Args&&...) noexcept {}
};

template<bool Condition, class Base>
struct or_empty
:
        std::conditional_t<Condition, Base, empty<Base>>
{
        static_assert(std::is_empty_v<empty<Base>>);
};

}       // namespace block_adl

using block_adl::or_empty;

}       // namespace xstd
