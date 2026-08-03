//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_ARITHMETIC_LIKE_HPP
#define XSTD_TYPE_TRAITS_IS_ARITHMETIC_LIKE_HPP

#include <xstd/type_traits/is_integral_like.hpp>
#include <type_traits>

namespace xstd {

template<class T>
inline constexpr auto is_arithmetic_like_v = is_integral_like_v<T> or std::is_floating_point_v<T>;

template<class T>
using is_arithmetic_like = std::bool_constant<is_arithmetic_like_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_ARITHMETIC_LIKE_HPP
