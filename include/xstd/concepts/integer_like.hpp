//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGER_LIKE_HPP
#define XSTD_CONCEPTS_INTEGER_LIKE_HPP

#include <xstd/concepts/integer_class.hpp>    // integer_class
#include <xstd/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <concepts>                           // integral, same_as
#include <type_traits>                        // remove_cv_t

namespace xstd {

// [iterator.concept.winc]/1 first, then the pair [basic.fundamental]/2 gives every integer type.
template<class T>
concept integer_like =
        (not std::same_as<std::remove_cv_t<T>, bool>) and
        (std::integral<T> or integer_class<T>) and
        // The traits propagate cv from the succinct unqualified user specializations.
        requires {
                typename make_signed_t<T>;
                typename make_unsigned_t<T>;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_LIKE_HPP
