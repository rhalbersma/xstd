//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGER_CLASS_HPP
#define XSTD_CONCEPTS_INTEGER_CLASS_HPP

#include <compare>     // strong_ordering
#include <concepts>    // constructible_from, integral, regular, same_as, three_way_comparable
#include <cstddef>     // size_t
#include <limits>      // numeric_limits
#include <type_traits> // is_same_v, remove_cv_t

// [iterator.concept.winc]'s integer-class type, which integer_like admits alongside integral.
namespace xstd {

// /2's "behave as integer types do", spelled out. The term, and only the term:
// the pair this library divides with is integer_like's to ask for, not this concept's.
template<class T, class I = std::remove_cv_t<T>>
concept integer_class =
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
        // /6 the other way, direct-initialization being what a cast does; only size_t a body performs.
        std::constructible_from<int, I> and
        std::constructible_from<std::size_t, I> and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_CLASS_HPP
