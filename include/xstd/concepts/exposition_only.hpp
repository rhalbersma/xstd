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
                // I exactly, /7.3 continuing "if @x has type B(I), then @a has
                // type I". Named without a static_cast<I> around it: that cast
                // is of type I whenever it compiles, which leaves same_as<I>
                // vacuous and the row asking no more than explicit
                // convertibility - and lets in a type whose operators hand back
                // an expression template, exactly what /7.3 rules out. The cast
                // was there for the promotion of narrow operands, int8_t +
                // int8_t being an int, and no operand of type I can promote:
                // integral types are excluded above, and /3 leaves an
                // integer-class type wider than every integral type of its
                // signedness.
                { +a } -> std::same_as<I>;
                { -a } -> std::same_as<I>;
                { ~a } -> std::same_as<I>;
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
        // I exactly again, /7.6 doing for these what /7.3 does for the unary
        // ones, and unwrapped for the same reason. Fenced off because
        // clang-format reads "a * b" and "a & b" here as declarations of a
        // pointer and a reference, and closes the space up accordingly.
        // clang-format off
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
        // clang-format on
        requires (I const a, std::size_t const n) {
                { a << n } -> std::same_as<I>;
                { a >> n } -> std::same_as<I>;
        } and
        // /8, contextually convertible to bool, which is also what gives a
        // type the && and || that /7.6 asks of every binary operator. The cast
        // stays: /6 makes the conversion explicit, so there is no expression to
        // check without one.
        requires (I const a) {
                { static_cast<bool>(a) } -> std::same_as<bool>;
        } and
        // /9's two concepts, which carry the comparisons: boolean-testable
        // results rather than /7.6's bool, and as good as bool at every use.
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        // /6 the other way round, integral to integer-class, in the spelling
        // <xstd/cstdlib.hpp> uses for its constants: "a < static_cast<I>(0)"
        // compiles for every I here where "a < 0" need not.
        requires {
                { static_cast<I>(0) } -> std::same_as<I>;
        } and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
