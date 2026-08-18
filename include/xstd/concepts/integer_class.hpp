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
#include <type_traits> // remove_cv_t

// [iterator.concept.winc]'s integer-class type, which integer admits alongside integral.
namespace xstd {

// /2's "behave as integer types do", spelled out; the term alone, the pair being integer's.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept integer_class =
        // T is the parameter's own default; naming it explicitly cannot redirect the question.
        std::same_as<T, std::remove_cv_t<T_cv>> and
        // Entailed by /3's width clause; without it int and short get in.
        (not std::integral<T>) and
        requires { sizeof(T); } and
        // /6 one way: an integral value converts to an integer-class type.
        std::constructible_from<T, int> and
        // Then increment, unary, compound assignment and binary operators, in order.
        requires (T a) {
                { a++ } -> std::same_as<T>;
                { a-- } -> std::same_as<T>;
                { ++a } -> std::same_as<T&>;
                { --a } -> std::same_as<T&>;
        } and
        requires (T const a) {
                // T exactly, per /7.3; a static_cast<T> here would admit an expression template.
                { +a } -> std::same_as<T>;
                { -a } -> std::same_as<T>;
                { ~a } -> std::same_as<T>;
                // bool exactly: /7.3's sentence about `!` alone.
                { not a } -> std::same_as<bool>;
        } and
        requires (T a, T const b) {
                { a *= b } -> std::same_as<T&>;
                { a /= b } -> std::same_as<T&>;
                { a %= b } -> std::same_as<T&>;
                { a += b } -> std::same_as<T&>;
                { a -= b } -> std::same_as<T&>;
                { a &= b } -> std::same_as<T&>;
                { a ^= b } -> std::same_as<T&>;
                { a |= b } -> std::same_as<T&>;
        } and
        requires (T a, std::size_t const n) {
                { a <<= n } -> std::same_as<T&>;
                { a >>= n } -> std::same_as<T&>;
        } and
        // T exactly again, per /7.6.
        requires (T const a, T const b) {
                { a * b } -> std::same_as<T>;
                { a / b } -> std::same_as<T>;
                { a % b } -> std::same_as<T>;
                { a + b } -> std::same_as<T>;
                { a - b } -> std::same_as<T>;
                { a & b } -> std::same_as<T>;
                { a ^ b } -> std::same_as<T>;
                { a | b } -> std::same_as<T>;
        } and
        requires (T const a, std::size_t const n) {
                { a << n } -> std::same_as<T>;
                { a >> n } -> std::same_as<T>;
        } and
        // /8, contextually convertible to bool; the cast stays, /6 making it explicit.
        requires (T const a) {
                { static_cast<bool>(a) } -> std::same_as<bool>;
        } and
        // /9's two concepts carry the comparisons, boolean-testable rather than bool.
        std::regular<T> and
        std::three_way_comparable<T, std::strong_ordering> and
        // /6 the other way, direct-initialization being what a cast does; only size_t a body performs.
        std::constructible_from<int, T> and
        std::constructible_from<std::size_t, T> and
        std::numeric_limits<T>::is_specialized and
        std::numeric_limits<T>::is_integer;

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_CLASS_HPP
