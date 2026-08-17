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
template<class I>
concept integer_like =
        (not std::same_as<std::remove_cv_t<I>, bool>) and
        (std::integral<I> or integer_class<I>) and
        // Asked with the cv off, per /11: a user specializes for the type, not for a const of it.
        requires {
                typename make_signed_t<std::remove_cv_t<I>>;
                typename make_unsigned_t<std::remove_cv_t<I>>;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_LIKE_HPP
