//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGER_HPP
#define XSTD_CONCEPTS_INTEGER_HPP

#include <xstd/concepts/integer_class.hpp>    // integer_class
#include <xstd/type_traits/is_character.hpp>  // is_character
#include <xstd/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <concepts>                           // integral, same_as
#include <type_traits>                        // remove_cv_t

namespace xstd {

// P3701R0's arithmetic boundary, extended with xstd's paired integer-class types.
template<class T>
concept integer =
        (not is_character_v<std::remove_cv_t<T>>) and
        (not std::same_as<std::remove_cv_t<T>, bool>) and
        (std::integral<T> or integer_class<T>) and
        requires {
                typename make_signed_t<T>;
                typename make_unsigned_t<T>;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_HPP
