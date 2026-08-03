//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP
#define XSTD_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP

#include <type_traits>

namespace xstd {

template<class T, class U>
inline constexpr auto is_integral_constant_v = false;

template<class U, U N>
inline constexpr auto is_integral_constant_v<std::integral_constant<U, N>, U> = true;

template<class T, class U>
using is_integral_constant = std::bool_constant<is_integral_constant_v<T, U>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP
