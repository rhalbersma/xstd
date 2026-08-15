//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_SIGNED_INTEGRAL_LIKE_HPP
#define XSTD_CONCEPTS_SIGNED_INTEGRAL_LIKE_HPP

#include <xstd/concepts/integral_like.hpp>     // integral_like
#include <xstd/type_traits/is_signed_like.hpp> // is_signed_like_v

namespace xstd {

// Mirrors std::signed_integral, keeping integral_like atomic for subsumption.
template<class T>
concept signed_integral_like = integral_like<T> and is_signed_like_v<T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_SIGNED_INTEGRAL_LIKE_HPP
