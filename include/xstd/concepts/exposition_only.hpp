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
#include <type_traits> // is_same_v, remove_cv_t

// Internal concepts used to define xstd's public numeric traits and concepts.
namespace xstd::exposition_only {

// [iterator.concept.winc], asked of I: T with the cv off, per /11. Nothing needs noexcept.
template<class T, class I = std::remove_cv_t<T>>
concept integer_class_type =
        // I is the parameter's own default; naming it explicitly cannot redirect the question.
        std::is_same_v<I, std::remove_cv_t<T>> and
        // Entailed by /3's width clause; without it int and short get in.
        (not std::integral<I>) and
        requires { sizeof(I); } and
        // /6 one way: an integral value converts to an integer-class type.
        std::constructible_from<I, int> and
        // Then increment, unary, compound assignment and binary operators, in order.
        requires (I a) {
                { a++ } -> std::same_as<I>;
                { a-- } -> std::same_as<I>;
                { ++a } -> std::same_as<I&>;
                { --a } -> std::same_as<I&>;
        } and
        requires (I const a) {
                // I exactly, per /7.3; a static_cast<I> here would admit an expression template.
                { +a } -> std::same_as<I>;
                { -a } -> std::same_as<I>;
                { ~a } -> std::same_as<I>;
                // bool exactly: /7.3's sentence about `!` alone.
                { not a } -> std::same_as<bool>;
        } and
        requires (I a, I const b) {
                { a *= b } -> std::same_as<I&>;
                { a /= b } -> std::same_as<I&>;
                { a %= b } -> std::same_as<I&>;
                { a += b } -> std::same_as<I&>;
                { a -= b } -> std::same_as<I&>;
                { a &= b } -> std::same_as<I&>;
                { a ^= b } -> std::same_as<I&>;
                { a |= b } -> std::same_as<I&>;
        } and
        requires (I a, std::size_t const n) {
                { a <<= n } -> std::same_as<I&>;
                { a >>= n } -> std::same_as<I&>;
        } and
        // I exactly again, per /7.6.
        requires (I const a, I const b) {
                { a * b } -> std::same_as<I>;
                { a / b } -> std::same_as<I>;
                { a % b } -> std::same_as<I>;
                { a + b } -> std::same_as<I>;
                { a - b } -> std::same_as<I>;
                { a & b } -> std::same_as<I>;
                { a ^ b } -> std::same_as<I>;
                { a | b } -> std::same_as<I>;
        } and
        requires (I const a, std::size_t const n) {
                { a << n } -> std::same_as<I>;
                { a >> n } -> std::same_as<I>;
        } and
        // /8, contextually convertible to bool; the cast stays, /6 making it explicit.
        requires (I const a) {
                { static_cast<bool>(a) } -> std::same_as<bool>;
        } and
        // /9's two concepts carry the comparisons, boolean-testable rather than bool.
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        // The same direction again, in the spelling <xstd/cstdlib.hpp> uses for its constants.
        requires {
                { static_cast<I>(0) } -> std::same_as<I>;
        } and
        // /6 the other way: to any integral type, these two standing for "any" by sign and width.
        requires (I const a) {
                { static_cast<int>(a) } -> std::same_as<int>;
                { static_cast<std::size_t>(a) } -> std::same_as<std::size_t>;
        } and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
