//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_NOTHROW_INTEGER_OPERATORS_HPP
#define XSTD_CONCEPTS_NOTHROW_INTEGER_OPERATORS_HPP

#include <xstd/concepts/integer_like.hpp> // integer_like
#include <concepts>                       // same_as
#include <cstddef>                        // size_t
#include <type_traits>                    // remove_cv_t

namespace xstd {

// Whether a type's const operations all carry noexcept, asked of I with the cv stripped.
template<class T, class I = std::remove_cv_t<T>>
concept nothrow_integer_operators =
        // I is the parameter's own default; naming it explicitly cannot redirect the question.
        std::same_as<I, std::remove_cv_t<T>> and
        integer_like<I> and
        // The conversions themselves: /6 and /8 make both explicit, so the cast is the expression.
        requires {
                { static_cast<I>(0) } noexcept;
        } and
        requires (I const a) {
                { static_cast<bool>(a) } noexcept;
                // Asked as written: /7.3 and /7.6 give the result type already, so a cast adds nothing.
                { +a } noexcept;
                { -a } noexcept;
                { ~a } noexcept;
                { not a } noexcept;
        } and
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

#endif // XSTD_CONCEPTS_NOTHROW_INTEGER_OPERATORS_HPP
