//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_NOTHROW_ARITHMETIC_HPP
#define XSTD_CONCEPTS_NOTHROW_ARITHMETIC_HPP

#include <xstd/concepts/integral_like.hpp> // integral_like
#include <cstddef>                         // size_t

namespace xstd {

// Whether an integral-like type can pass a noexcept on. Public because it is
// the exception specification of xstd::abs, unsigned_abs, sign, div,
// euclidean_div and floored_div: a caller who wants to know whether one of
// those throws over a type of their own is asking this question, and had no
// way to spell it while the answer lived in an exposition-only namespace.
//
// True for every integral type, and for an integer-class type whose author
// wrote the specifier; false for one that merely happens not to throw, which
// is not something a declaration can be asked. [iterator.concept.winc] never
// requires it - the word does not occur in the subclause - so refusing an
// unannotated type would narrow the extension point rather than the exception
// specification. See exposition_only::integer_class_type.
//
// The operations are every requirement integer_class_type states over const
// operands, and no more. That boundary is not this library's invention:
// absl::uint128 declares each of these constexpr and not one of its mutating
// operators - ++, --, and the twelve compound assignments - so the const half
// is what an integer-class type in the field treats as its value surface.
// It is also the only half these six functions can reach, since each of them
// takes its argument by value and returns a value.
//
// Within that half the coverage is total rather than itemized, which is what
// keeps it honest. One predicate for all six functions, listing operations
// none of them performs: reporting noexcept(false) over an operation a
// function does not use is only over-cautious, while missing one it does use
// is a wrong noexcept(true), and a list drawn from what the bodies happen to
// contain today has to be revisited every time a body changes. This one does
// not - `div` alone reaches *, +, -, /, %, == and != through its assertions.
//
// A stand-in, in other words, for noexcept(auto), which would take each
// function's specification from its own definition and leave nothing here to
// get wrong. That was N3207 in 2010, is P3166 today, and is in no standard
// yet; see doc/design.md.
template<class T>
concept nothrow_arithmetic =
        integral_like<T> and
        requires {
                { static_cast<T>(0) } noexcept;
        } and
        requires (T const a) {
                { static_cast<T>(+a) } noexcept;
                { static_cast<T>(-a) } noexcept;
                { static_cast<T>(~a) } noexcept;
                { not a } noexcept;
        } and
        requires (T const a, T const b) {
                { static_cast<T>(a + b) } noexcept;
                { static_cast<T>(a - b) } noexcept;
                { static_cast<T>(a * b) } noexcept;
                { static_cast<T>(a / b) } noexcept;
                { static_cast<T>(a % b) } noexcept;
                { static_cast<T>(a & b) } noexcept;
                { static_cast<T>(a | b) } noexcept;
                { static_cast<T>(a ^ b) } noexcept;
        } and
        requires (T const a, std::size_t const n) {
                { static_cast<T>(a << n) } noexcept;
                { static_cast<T>(a >> n) } noexcept;
        } and
        requires (T const a, T const b) {
                { a == b } noexcept;
                { a != b } noexcept;
                { a < b } noexcept;
                { a > b } noexcept;
                { a <= b } noexcept;
                { a >= b } noexcept;
                { a <=> b } noexcept;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_NOTHROW_ARITHMETIC_HPP
