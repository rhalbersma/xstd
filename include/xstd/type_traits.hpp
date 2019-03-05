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
constexpr auto to_underlying_type(Enumeration e) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(e);
}

template<class Enumeration, Enumeration N, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_underlying_type(std::integral_constant<Enumeration, N>) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(N);
}

template<class T, class... Args>
constexpr auto is_any_of = (std::is_same_v<T, Args...> || ...);

namespace block_adl {

template<class Tag>
struct tagged_empty
{
        tagged_empty() = default;

        template<class... Args>
        constexpr explicit tagged_empty(Args&&...) noexcept {}
};

template<bool Condition, class Base>
struct conditional_empty
:
        std::conditional_t<Condition, Base, tagged_empty<Base>>
{
        static_assert(std::is_empty_v<tagged_empty<Base>>);
};

}       // namespace block_adl

using block_adl::conditional_empty;

}       // namespace xstd
