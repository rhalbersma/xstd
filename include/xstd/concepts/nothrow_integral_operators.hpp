//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_NOTHROW_INTEGRAL_OPERATORS_HPP
#define XSTD_CONCEPTS_NOTHROW_INTEGRAL_OPERATORS_HPP

#include <xstd/concepts/integral_like.hpp> // integral_like
#include <cstddef>                         // size_t
#include <type_traits>                     // remove_cv_t

namespace xstd {

// Whether an integral-like type can pass a noexcept on, over every operation
// [iterator.concept.winc] asks of const operands. See doc/design.md.
//
// Asked of I: T with the cv-qualification off, in integer_class_type's
// spelling and for its reason. Every row below is stated over a const operand,
// so const is transparent without help; volatile is not, and a type whose
// operators are declared const - which is how absl::uint128 and Boost.Int128
// declare theirs - answers no to every one of them under a volatile operand.
// Without the stripping the two branches of integral_like disagree: the
// built-in types and __int128 pass volatile on the language's own operators,
// where an integer-class type fails on its members.
template<class T, class I = std::remove_cv_t<T>>
concept nothrow_integral_operators =
        integral_like<I> and
        // The two conversions, which are the thing asked about here rather
        // than a wrapper around it: [iterator.concept.winc]/6 and /8 make both
        // explicit, so there is no expression to put a noexcept on without the
        // cast, and an integer-class type has to declare them for either to
        // hold.
        requires {
                { static_cast<I>(0) } noexcept;
        } and
        requires (I const a) {
                { static_cast<bool>(a) } noexcept;
                // The operators, asked as written. A static_cast<T> around one
                // adds nothing to what noexcept sees: /7.3 and /7.6 give the
                // result type T already, leaving the cast the identity, and
                // where I is a narrow integral type it is a built-in
                // conversion, which cannot throw either way.
                { +a } noexcept;
                { -a } noexcept;
                { ~a } noexcept;
                { not a } noexcept;
        } and
        // Fenced off for the reason integer_class_type's binary block is:
        // clang-format reads "a * b" and "a & b" in a requires-expression as
        // declarations of a pointer and a reference.
        // clang-format off
        requires (I const a, I const b) {
                { a * b } noexcept;
                { a / b } noexcept;
                { a % b } noexcept;
                { a + b } noexcept;
                { a - b } noexcept;
                { a & b } noexcept;
                { a ^ b } noexcept;
                { a | b } noexcept;
        } and
        // clang-format on
        requires (I const a, std::size_t const n) {
                { a << n } noexcept;
                { a >> n } noexcept;
        } and
        requires (I const a, I const b) {
                { a <=> b } noexcept;
                { a < b } noexcept;
                { a > b } noexcept;
                { a <= b } noexcept;
                { a >= b } noexcept;
                { a == b } noexcept;
                { a != b } noexcept;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_NOTHROW_INTEGRAL_OPERATORS_HPP
