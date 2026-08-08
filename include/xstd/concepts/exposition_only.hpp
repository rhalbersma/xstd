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

// The integer-class requirements of [iterator.concept.winc], asked of I: T with
// the cv-qualification off, per /11. Nothing here is required to be noexcept.
template<class T, class I = std::remove_cv_t<T>>
concept integer_class_type =
        // Unstated by the subclause but entailed by /3, whose width clause no
        // integral type meets against itself; without it int and short get in.
        (not std::integral<I>) and
        requires { sizeof(I); } and
        // [iterator.concept.winc] first specifies conversions between integral
        // and integer-class types.
        std::constructible_from<I, int> and
        // It then specifies increment/decrement, unary operators, compound
        // assignment, and non-assignment binary operators, in that order.
        requires (I a) {
                { a++ } -> std::same_as<I>;
                { a-- } -> std::same_as<I>;
                { ++a } -> std::same_as<I&>;
                { --a } -> std::same_as<I&>;
        } and
        requires (I const a) {
                { static_cast<I>(+a) } -> std::same_as<I>;
                { static_cast<I>(-a) } -> std::same_as<I>;
                { static_cast<I>(~a) } -> std::same_as<I>;
                // bool exactly: /7.3 says "if @x has type bool, so too does
                // @a", which of these four is about `!` alone.
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
        requires (I const a, I const b) {
                { static_cast<I>(a * b) } -> std::same_as<I>;
                { static_cast<I>(a / b) } -> std::same_as<I>;
                { static_cast<I>(a % b) } -> std::same_as<I>;
                { static_cast<I>(a + b) } -> std::same_as<I>;
                { static_cast<I>(a - b) } -> std::same_as<I>;
                { static_cast<I>(a & b) } -> std::same_as<I>;
                { static_cast<I>(a ^ b) } -> std::same_as<I>;
                { static_cast<I>(a | b) } -> std::same_as<I>;
        } and
        requires (I const a, std::size_t const n) {
                { static_cast<I>(a << n) } -> std::same_as<I>;
                { static_cast<I>(a >> n) } -> std::same_as<I>;
        } and
        requires (I const a, I const b) {
                // Not left to /9: /7.6's "any other type" clause has these to
                // describe or nothing, and it asks more than boolean-testable.
                { a <=> b } -> std::same_as<std::strong_ordering>;
                { a < b } -> std::same_as<bool>;
                { a > b } -> std::same_as<bool>;
                { a <= b } -> std::same_as<bool>;
                { a >= b } -> std::same_as<bool>;
                { a == b } -> std::same_as<bool>;
                { a != b } -> std::same_as<bool>;
        } and
        // Finally /9's two concepts - only regular of them still structural -
        // and /10 and /11.
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        requires {
                { static_cast<I>(0) } -> std::same_as<I>;
        } and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
