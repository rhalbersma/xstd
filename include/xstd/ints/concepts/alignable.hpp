//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_ALIGNABLE_HPP
#define XSTD_INTS_CONCEPTS_ALIGNABLE_HPP

#include <xstd/ints/cstdint/bit_int.hpp>       // IWYU pragma: keep; the _BitInt numeric_limits specializations
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <concepts>                            // constructible_from, convertible_to, totally_ordered
#include <cstddef>                             // size_t
#include <type_traits>                         // is_nothrow_constructible_v

namespace xstd {

// A prefix of integer_class: what alignment needs of a type, and nothing beyond it.
template<class T>
concept alignable =
        // /2-/3, where integer_class asks it; radix 2 is what licenses & for the remainder.
        numeric_limits<T>::is_specialized and
        numeric_limits<T>::is_integer and
        (numeric_limits<T>::radix == 2) and

        // Modular rather than signed, so a wrapped sum is a value the precondition can ask about.
        not numeric_limits<T>::is_signed and

        // Wider than one bit, which is what bool is not: its one plus one is still one.
        (numeric_limits<T>::digits > 1) and

        // /6, both ways: an address goes in and an address comes back out.
        std::convertible_to<std::size_t, T> and
        std::constructible_from<std::size_t, T> and

        // /9, the comparison align_up's overflow precondition is stated with.
        std::totally_ordered<T> and

        // The arithmetic itself: alignment is addition modulo a power of two.
        requires (T x) {
                static_cast<T>(x + x);
                static_cast<T>(x - x);
                static_cast<T>(x & x);
        };

// Whether those operations carry noexcept, which Abseil's 128-bit types do not, being alignable all the same.
template<class T>
concept nothrow_alignable =
        alignable<T> and
        std::is_nothrow_constructible_v<T, std::size_t> and
        std::is_nothrow_constructible_v<std::size_t, T> and
        requires (T x) {
                { static_cast<T>(x + x) } noexcept;
                { static_cast<T>(x - x) } noexcept;
                { static_cast<T>(x & x) } noexcept;
                { x == x } noexcept;
                { x >= x } noexcept;
        };

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_ALIGNABLE_HPP
