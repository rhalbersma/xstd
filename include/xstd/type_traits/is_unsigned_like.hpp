//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_UNSIGNED_LIKE_HPP
#define XSTD_TYPE_TRAITS_IS_UNSIGNED_LIKE_HPP

#include <xstd/type_traits/is_arithmetic_like.hpp>
#include <type_traits>

namespace xstd {

template<class T>
inline constexpr auto is_unsigned_like_v = false;

template<class T>
        requires is_arithmetic_like_v<T>
// NOLINTNEXTLINE(readability-implicit-bool-conversion,modernize-use-bool-literals)
inline constexpr auto is_unsigned_like_v<T> = static_cast<T>(0) < static_cast<T>(-1);

template<class T>
using is_unsigned_like = std::bool_constant<is_unsigned_like_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_UNSIGNED_LIKE_HPP
