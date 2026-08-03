//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_EXPOSITION_ONLY_INTEGRAL_CLASS_HPP
#define XSTD_CONCEPTS_EXPOSITION_ONLY_INTEGRAL_CLASS_HPP

#include <compare>     // strong_ordering
#include <concepts>    // constructible_from, integral, regular, same_as, three_way_comparable
#include <cstddef>     // size_t
#include <limits>      // numeric_limits
#include <type_traits> // remove_cv_t

// Internal concepts used to define xstd's public numeric traits and concepts.
namespace xstd::exposition_only {

// Structural form of the integer-class requirements in [iterator.concept.winc].
template<class I>
concept integral_class_type =
        (not std::integral<I>) and
        requires { sizeof(I); } and
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer and
        std::constructible_from<I, int> and
        requires {
                { static_cast<I>(0) } noexcept -> std::same_as<I>;
        } and
        requires (I const a) {
                { static_cast<I>(+a) } noexcept -> std::same_as<I>;
                { static_cast<I>(-a) } noexcept -> std::same_as<I>;
                { static_cast<I>(~a) } noexcept -> std::same_as<I>;
        } and
        requires (I const a, I const b) {
                { static_cast<I>(a + b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a - b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a * b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a / b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a % b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a & b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a | b) } noexcept -> std::same_as<I>;
                { static_cast<I>(a ^ b) } noexcept -> std::same_as<I>;
                { a == b } noexcept -> std::same_as<bool>;
                { a <=> b } noexcept -> std::same_as<std::strong_ordering>;
        } and
        requires (I a, I const b) {
                { a += b } noexcept -> std::same_as<I&>;
                { a -= b } noexcept -> std::same_as<I&>;
                { a *= b } noexcept -> std::same_as<I&>;
                { a /= b } noexcept -> std::same_as<I&>;
                { a %= b } noexcept -> std::same_as<I&>;
                { a &= b } noexcept -> std::same_as<I&>;
                { a |= b } noexcept -> std::same_as<I&>;
                { a ^= b } noexcept -> std::same_as<I&>;
        } and
        requires (I a) {
                { ++a } noexcept -> std::same_as<I&>;
                { a++ } noexcept -> std::same_as<I>;
                { --a } noexcept -> std::same_as<I&>;
                { a-- } noexcept -> std::same_as<I>;
        } and
        requires (I const a, std::size_t n) {
                { static_cast<I>(a << n) } noexcept -> std::same_as<I>;
                { static_cast<I>(a >> n) } noexcept -> std::same_as<I>;
        } and
        requires (I a, std::size_t n) {
                { a <<= n } noexcept -> std::same_as<I&>;
                { a >>= n } noexcept -> std::same_as<I&>;
        };

// Unlike the iterator-only concept, this includes bool to widen std::integral.
template<class I>
concept is_integral_like = std::integral<I> or integral_class_type<I>;

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_INTEGRAL_CLASS_HPP
