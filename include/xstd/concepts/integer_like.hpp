//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGER_LIKE_HPP
#define XSTD_CONCEPTS_INTEGER_LIKE_HPP

#include <xstd/concepts/exposition_only.hpp> // integer_class_type
#include <concepts>                          // integral, same_as
#include <type_traits>                       // remove_cv_t

namespace xstd {

// [iterator.concept.winc]/1 as written: a type other than cv bool, integral or integer-class.
template<class T>
concept integer_like = (not std::same_as<std::remove_cv_t<T>, bool>) and
                       (std::integral<T> or exposition_only::integer_class_type<T>);

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_LIKE_HPP
