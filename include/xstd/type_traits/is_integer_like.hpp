//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_INTEGER_LIKE_HPP
#define XSTD_TYPE_TRAITS_IS_INTEGER_LIKE_HPP

#include <xstd/concepts/integer_like.hpp> // integer_like
#include <type_traits>                    // bool_constant

namespace xstd {

// The trait spelling of xstd::integer_like.
template<class T>
inline constexpr auto is_integer_like_v = integer_like<T>;

template<class T>
using is_integer_like = std::bool_constant<is_integer_like_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_INTEGER_LIKE_HPP
