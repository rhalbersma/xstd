//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_UNSIGNED_INTEGRAL_LIKE_HPP
#define XSTD_CONCEPTS_UNSIGNED_INTEGRAL_LIKE_HPP

#include <xstd/concepts/integral_like.hpp>       // integral_like
#include <xstd/type_traits/is_unsigned_like.hpp> // is_unsigned_like_v

namespace xstd {

template<class T>
concept unsigned_integral_like = integral_like<T> and is_unsigned_like_v<T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_UNSIGNED_INTEGRAL_LIKE_HPP
