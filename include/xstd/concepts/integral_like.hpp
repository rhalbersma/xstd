//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGRAL_LIKE_HPP
#define XSTD_CONCEPTS_INTEGRAL_LIKE_HPP

#include <xstd/concepts/exposition_only.hpp>

namespace xstd {

template<class T>
concept integral_like = exposition_only::is_integer_like<T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGRAL_LIKE_HPP
