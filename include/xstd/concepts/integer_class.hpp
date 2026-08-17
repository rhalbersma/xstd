//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGER_CLASS_HPP
#define XSTD_CONCEPTS_INTEGER_CLASS_HPP

#include <xstd/concepts/integer_class_operations.hpp> // integer_class_operations
#include <xstd/type_traits/make_signed.hpp>           // make_signed_t
#include <xstd/type_traits/make_unsigned.hpp>         // make_unsigned_t
#include <type_traits>                                // remove_cv_t

// [iterator.concept.winc]'s integer-class type, which integer_like admits alongside integral.
namespace xstd {

// The subclause's operations, and the pair [basic.fundamental]/2 gives every integer type.
template<class T>
concept integer_class =
        integer_class_operations<T> and
        // Asked with the cv off, per /11: a user specializes for the type, not for a const of it.
        requires {
                typename make_signed_t<std::remove_cv_t<T>>;
                typename make_unsigned_t<std::remove_cv_t<T>>;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_CLASS_HPP
