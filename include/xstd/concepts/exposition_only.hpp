//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
#define XSTD_CONCEPTS_EXPOSITION_ONLY_HPP

#include <compare>     // strong_ordering
#include <concepts>    // constructible_from, integral, regular, same_as, three_way_comparable
#include <cstddef>     // size_t
#include <limits>      // numeric_limits
#include <type_traits> // remove_cv_t

// Internal concepts used to define xstd's public numeric traits and concepts.
namespace xstd::exposition_only {

// Structural form of the integer-class requirements in [iterator.concept.winc].
template<class I>
concept integer_class_type =
        (not std::integral<I>) and
        requires { sizeof(I); } and
        // [iterator.concept.winc] first specifies conversions between integral
        // and integer-class types.
        std::constructible_from<I, int> and
        // It then specifies increment/decrement, unary operators, compound
        // assignment, and non-assignment binary operators, in that order.
        requires (I a) {
                { a++ } noexcept -> std::same_as<I>;
                { a-- } noexcept -> std::same_as<I>;
                { ++a } noexcept -> std::same_as<I&>;
                { --a } noexcept -> std::same_as<I&>;
        } and
        requires (I const a) {
                { static_cast<I>(+a) } noexcept -> std::same_as<I>;
                { static_cast<I>(-a) } noexcept -> std::same_as<I>;
                { static_cast<I>(~a) } noexcept -> std::same_as<I>;
                { not a } noexcept -> std::same_as<bool>;
        } and
        requires (I a, I const b, std::size_t n) {
                { a += b } noexcept -> std::same_as<I&>;
                { a -= b } noexcept -> std::same_as<I&>;
                { a *= b } noexcept -> std::same_as<I&>;
                { a /= b } noexcept -> std::same_as<I&>;
                { a %= b } noexcept -> std::same_as<I&>;
                { a &= b } noexcept -> std::same_as<I&>;
                { a |= b } noexcept -> std::same_as<I&>;
                { a ^= b } noexcept -> std::same_as<I&>;
                { a <<= n } noexcept -> std::same_as<I&>;
                { a >>= n } noexcept -> std::same_as<I&>;
        } and
        requires (I const a, I const b, std::size_t n) {
                { static_cast<I>(a + b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a - b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a * b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a / b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a % b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a & b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a | b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a ^ b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a << n) } noexcept -> std::same_as<I>;
                { static_cast<I>(a >> n) } noexcept -> std::same_as<I>;
                { a == b } noexcept -> std::same_as<bool>;
                { a <=> b } noexcept -> std::same_as<std::strong_ordering>;
        } and
        // Finally come regularity and ordering, value-initialization, and
        // numeric_limits.
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        requires {
                { static_cast<I>(0) } noexcept -> std::same_as<I>;
        } and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

// This follows the Standard's exposition-only is-integer-like exactly: cv bool
// is excluded even though it models std::integral.
template<class I>
concept is_integer_like =
        (not std::same_as<std::remove_cv_t<I>, bool>) and
        (std::integral<I> or integer_class_type<I>);

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
