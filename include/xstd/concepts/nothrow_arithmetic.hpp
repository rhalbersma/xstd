//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_NOTHROW_ARITHMETIC_HPP
#define XSTD_CONCEPTS_NOTHROW_ARITHMETIC_HPP

#include <xstd/concepts/integral_like.hpp> // integral_like
#include <cstddef>                         // size_t

namespace xstd {

// Whether an integral-like type can pass a noexcept on, over every operation
// [iterator.concept.winc] asks of const operands. See doc/design.md.
template<class T>
concept nothrow_arithmetic =
        integral_like<T> and
        requires {
                { static_cast<T>(0) } noexcept;
        } and
        requires (T const a) {
                { static_cast<bool>(a) } noexcept;
                { static_cast<T>(+a) } noexcept;
                { static_cast<T>(-a) } noexcept;
                { static_cast<T>(~a) } noexcept;
                { not a } noexcept;
        } and
        requires (T const a, T const b) {
                { static_cast<T>(a * b) } noexcept;
                { static_cast<T>(a / b) } noexcept;
                { static_cast<T>(a % b) } noexcept;
                { static_cast<T>(a + b) } noexcept;
                { static_cast<T>(a - b) } noexcept;
                { static_cast<T>(a & b) } noexcept;
                { static_cast<T>(a ^ b) } noexcept;
                { static_cast<T>(a | b) } noexcept;
        } and
        requires (T const a, std::size_t const n) {
                { static_cast<T>(a << n) } noexcept;
                { static_cast<T>(a >> n) } noexcept;
        } and
        requires (T const a, T const b) {
                { a <=> b } noexcept;
                { a < b } noexcept;
                { a > b } noexcept;
                { a <= b } noexcept;
                { a >= b } noexcept;
                { a == b } noexcept;
                { a != b } noexcept;
        };

} // namespace xstd

#endif // XSTD_CONCEPTS_NOTHROW_ARITHMETIC_HPP
