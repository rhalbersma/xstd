//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGRAL_LIKE_HPP
#define XSTD_CONCEPTS_INTEGRAL_LIKE_HPP

#include <xstd/concepts/exposition_only.hpp> // integer_class_type

namespace xstd {

// The open counterpart of std::integral, admitting integer-class types.
template<class T>
concept integral_like = std::integral<T> or exposition_only::integer_class_type<T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGRAL_LIKE_HPP
