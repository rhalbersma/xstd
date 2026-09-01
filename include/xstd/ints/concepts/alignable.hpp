//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_ALIGNABLE_HPP
#define XSTD_INTS_CONCEPTS_ALIGNABLE_HPP

#include <xstd/ints/cstdint/bit_int.hpp>       // IWYU pragma: keep; the _BitInt numeric_limits specializations
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <concepts>                            // constructible_from, convertible_to, same_as, totally_ordered
#include <cstddef>                             // size_t
#include <type_traits>                         // is_nothrow_constructible_v, remove_cv_t

namespace xstd {

// A prefix of integer_class: what alignment needs of a type, and nothing beyond it.
// Asked of T with the cv stripped, as integer_class and its refinements are: a
// qualified built-in loses its qualifiers to the lvalue-to-rvalue conversion on the
// way to operator+, and a qualified class type would otherwise have no operator at
// all, so the answer would turn on whether an implementation made the type a class.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept alignable =
        // T is the parameter's own default; naming it explicitly cannot redirect the question.
        std::same_as<T, std::remove_cv_t<T_cv>> and

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
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept nothrow_alignable =
        std::same_as<T, std::remove_cv_t<T_cv>> and
        alignable<T> and

        // The way out of every by-value return, which the casts below do not reach.
        // Not regularity: alignable asks totally_ordered and no more, so its nothrow
        // refinement asks noexcept of exactly that and adds no requirement of its own.
        std::is_nothrow_destructible_v<T> and

        std::is_nothrow_constructible_v<T, std::size_t> and
        std::is_nothrow_constructible_v<std::size_t, T> and
        requires (T x) {
                { static_cast<T>(x + x) } noexcept;
                { static_cast<T>(x - x) } noexcept;
                { static_cast<T>(x & x) } noexcept;
        } and

        // totally_ordered in full: the equality pair and all four relations, where
        // only == and >= were asked before.
        requires (T const a, T const b) {
                { a == b } noexcept;
                { a != b } noexcept;
                { a < b } noexcept;
                { a > b } noexcept;
                { a <= b } noexcept;
                { a >= b } noexcept;
        };

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_ALIGNABLE_HPP
