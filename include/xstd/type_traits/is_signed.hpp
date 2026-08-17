//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_SIGNED_HPP
#define XSTD_TYPE_TRAITS_IS_SIGNED_HPP

#include <xstd/concepts/integer_class.hpp> // integer_class
#include <type_traits>                     // bool_constant, is_signed_v

namespace xstd {

// std::is_signed, which already answers for every type it covers and false for the rest.
template<class T>
inline constexpr auto is_signed_v = std::is_signed_v<T>;

// Where std stops: -1 lands below zero only where there is room below zero.
template<integer_class I>
inline constexpr auto is_signed_v<I> = I(-1) < I(0);

template<class T>
using is_signed = std::bool_constant<is_signed_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_SIGNED_HPP
