//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_INTEGER_CLASS_HPP
#define XSTD_CONCEPTS_INTEGER_CLASS_HPP

#include <xstd/limits/numeric_limits.hpp> // numeric_limits
#include <compare>                        // strong_ordering
#include <concepts>                       // constructible_from, convertible_to, integral, regular, same_as, three_way_comparable
#include <cstddef>                        // size_t
#include <type_traits>                    // remove_cv_t

// [iterator.concept.winc]'s integer-class type, generalized to integers of any width.
namespace xstd {

template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept integer_class =
        // /2: structurally recognize a non-integral type rather than an implementation-defined set.
        std::same_as<T, std::remove_cv_t<T_cv>> and
        (not std::integral<T>) and

        // /3: xstd deliberately omits the requirement that the width exceed every integral type.
        numeric_limits<T>::is_specialized and
        numeric_limits<T>::is_integer and
        (numeric_limits<T>::radix == 2) and

        // /4: integer-like and its signedness refinements are defined from this concept.

        // /5: B(T) is a specification device and has no structural check.

        // /6: representative implicit conversion in and explicit conversions out.
        std::convertible_to<int, T> and
        requires (T const value) {
                static_cast<int>(value);
                static_cast<std::size_t>(value);
        } and

        // /7.1: postfix increment and decrement.
        requires (T a) {
                { a++ } -> std::same_as<T>;
                { a-- } -> std::same_as<T>;
        } and

        // /7.2: prefix increment and decrement.
        requires (T a) {
                { ++a } -> std::same_as<T&>;
                { --a } -> std::same_as<T&>;
        } and

        // /7.3: unary operators.
        requires (T const a) {
                { +a } -> std::same_as<T>;
                { -a } -> std::same_as<T>;
                { not a } -> std::same_as<bool>;
                { ~a } -> std::same_as<T>;
        } and

        // /7.4: xstd does not require mixed integral/integer-class compound assignment.

        // /7.5: same-type compound assignment, grouped by precedence and operand arity.
        requires (T a, T const b) {
                { a *= b } -> std::same_as<T&>;
                { a /= b } -> std::same_as<T&>;
                { a %= b } -> std::same_as<T&>;
                { a += b } -> std::same_as<T&>;
                { a -= b } -> std::same_as<T&>;
        } and
        requires (T a, std::size_t const n) {
                { a <<= n } -> std::same_as<T&>;
                { a >>= n } -> std::same_as<T&>;
        } and
        requires (T a, T const b) {
                { a &= b } -> std::same_as<T&>;
                { a ^= b } -> std::same_as<T&>;
                { a |= b } -> std::same_as<T&>;
        } and

        // /7.6: same-type binary operators, grouped by precedence and operand arity.
        requires (T const a, T const b) {
                { a * b } -> std::same_as<T>;
                { a / b } -> std::same_as<T>;
                { a % b } -> std::same_as<T>;
                { a + b } -> std::same_as<T>;
                { a - b } -> std::same_as<T>;
        } and
        requires (T const a, std::size_t const n) {
                { a << n } -> std::same_as<T>;
                { a >> n } -> std::same_as<T>;
        } and
        requires (T const a, T const b) {
                { a & b } -> std::same_as<T>;
                { a ^ b } -> std::same_as<T>;
                { a | b } -> std::same_as<T>;
        } and

        // /8: contextually convertible to bool; an explicit conversion is sufficient.
        std::constructible_from<bool, T> and

        // /9: regularity and strong ordering.
        std::regular<T> and
        std::three_way_comparable<T, std::strong_ordering> and

        // /10: semantic requirement — value initialization produces zero.

        // /11: the integer range functions return T.
        requires {
                { numeric_limits<T>::min() } -> std::same_as<T>;
                { numeric_limits<T>::max() } -> std::same_as<T>;
        };

// /12: xstd does not require mixed-mode common_type relationships.

} // namespace xstd

#endif // XSTD_CONCEPTS_INTEGER_CLASS_HPP
