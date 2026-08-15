//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_SIGN_HPP
#define XSTD_CSTDLIB_SIGN_HPP

#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators

namespace xstd {

// Returns -1, 0, or 1; over an unsigned type, 0 or 1.
//
// The one function here with no branch on signedness, because there is no
// theorem to state: "0 < x" less "x < 0" is the definition of the sign of a
// value, correct over any ordered type, rather than the signed algorithm with
// an arm that an unsigned instantiation cannot take.
template<integral_like I>
[[nodiscard]] constexpr auto sign(I x) noexcept(nothrow_integral_operators<I>)
        -> int
{
        auto const zero = static_cast<I>(0);
        return static_cast<int>(zero < x) - static_cast<int>(x < zero);
}

// Deleted as <xstd/charconv/to_chars.hpp> deletes it, and for its reason:
// bool is integral-like, and sign(true) would answer 1.
auto sign(bool) -> int = delete;

} // namespace xstd

#endif // XSTD_CSTDLIB_SIGN_HPP
