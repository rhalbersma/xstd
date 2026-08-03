//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_INTEGRAL_LIKE_HPP
#define XSTD_TYPE_TRAITS_IS_INTEGRAL_LIKE_HPP

#include <xstd/concepts/integral_like.hpp> // integral_like
#include <type_traits>                     // bool_constant

namespace xstd {

template<class T>
inline constexpr auto is_integral_like_v = integral_like<T>;

template<class T>
using is_integral_like = std::bool_constant<is_integral_like_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_INTEGRAL_LIKE_HPP
