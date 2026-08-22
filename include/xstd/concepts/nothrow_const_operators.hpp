//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_NOTHROW_CONST_OPERATORS_HPP
#define XSTD_CONCEPTS_NOTHROW_CONST_OPERATORS_HPP

#include <xstd/concepts/integer.hpp> // integer
#include <concepts>                  // same_as
#include <cstddef>                   // size_t
#include <type_traits>               // remove_cv_t

namespace xstd {

// Whether a type's const operations all carry noexcept, asked of T with the cv stripped.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept nothrow_const_operators =
        // T is the parameter's own default; naming it explicitly cannot redirect the question.
        std::same_as<T, std::remove_cv_t<T_cv>> and
        integer<T> and
        // The conversions themselves: /6 and /8 make both explicit, so the cast is the expression.
        requires {
                { static_cast<T>(0) } noexcept;
        } and
        requires (T const a) {
                { static_cast<bool>(a) } noexcept;
                // Asked as written: /7.3 and /7.6 give the result type already, so a cast adds nothing.
                { +a } noexcept;
                { -a } noexcept;
                { not a } noexcept;
                { ~a } noexcept;
        } and
        requires (T const a, T const b) {
                { a * b } noexcept;
                { a / b } noexcept;
                { a % b } noexcept;
                { a + b } noexcept;
                { a - b } noexcept;
                { a & b } noexcept;
                { a ^ b } noexcept;
                { a | b } noexcept;
        } and
        requires (T const a, std::size_t const n) {
                { a << n } noexcept;
                { a >> n } noexcept;
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

#endif // XSTD_CONCEPTS_NOTHROW_CONST_OPERATORS_HPP
