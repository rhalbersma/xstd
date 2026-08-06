//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_INT128_HPP
#define XSTD_FORMAT_INT128_HPP

#include <xstd/concepts/integral_like.hpp> // integral_like
#include <cstddef>                         // size_t
#include <limits>                          // numeric_limits

namespace xstd::detail {

// Decimal digits, a sign, and a null terminator. digits10 is the number of
// digits that always round-trip, so the value may need one more than that.
template<integral_like T>
inline constexpr auto decimal_buffer_size = static_cast<std::size_t>(std::numeric_limits<T>::digits10) + 3;

// Fills the buffer backwards from its end and returns the first character
// written, in the shape of <charconv>'s to_chars rather than a string-returning
// function. Two consequences, both of which xstd needs:
//
// - No allocation, so this is usable in a constant expression. That is what
//   lets std::formatter<div_t<S>>::format opt into P3391's constexpr
//   std::format; a std::to_string-based version never could.
// - One constrained template rather than a set of overloads on the concrete
//   128-bit types. An integer-class type that converts implicitly to the
//   built-in widths - Boost.Int128 is one - makes such overloads ambiguous,
//   while a template constrained on integral_like is an exact match and never
//   competes on conversions.
//
// The magnitude is never formed: negating the minimum value has no
// representation, and an integer-class type need not offer a wider one to
// borrow. Taking the remainder of the negative value and flipping the digit's
// sign avoids the question entirely, without assuming anything about how T
// represents itself.
template<integral_like T>
[[nodiscard]] constexpr auto to_decimal(char (&buffer)[decimal_buffer_size<T>], T value) noexcept
        -> char*
{
        auto last = buffer + decimal_buffer_size<T>;
        *--last = '\0';

        if (value == T{0}) {
                *--last = '0';
                return last;
        }

        auto const negative = value < T{0};
        for (auto rest = value; rest != T{0}; rest /= T{10}) {
                auto const digit = static_cast<int>(rest % T{10});
                *--last = static_cast<char>('0' + (negative ? -digit : digit));
        }
        if (negative) {
                *--last = '-';
        }
        return last;
}

} // namespace xstd::detail

#endif // XSTD_FORMAT_INT128_HPP
