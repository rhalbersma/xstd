//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_HPP
#define XSTD_CSTDLIB_DIV_HPP

#include <xstd/concepts/has_unsigned_counterpart.hpp>   // has_unsigned_counterpart
#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/cstdlib/div_t.hpp>                       // IWYU pragma: export; div_t
#include <xstd/cstdlib/sign.hpp>                        // sign
#include <xstd/cstdlib/unsigned_abs.hpp>                // unsigned_abs
#include <xstd/type_traits/is_signed_like.hpp>          // is_signed_like_v
#include <cassert>                                      // assert
#include <limits>                                       // numeric_limits

namespace xstd {

// Truncated division, as specified by [expr.mul].
template<integral_like I>
[[nodiscard]] constexpr auto div(I numer, I denom) noexcept(nothrow_integral_operators<I>)
        -> div_t<I>
{
        assert(denom != static_cast<I>(0));
        // Only a signed type has a MIN to reach it with; unsigned, this refuses div(0, max).
        if constexpr (is_signed_like_v<I>) {
                assert(numer != std::numeric_limits<I>::min() or denom != static_cast<I>(-1));
        }
        auto const qT = static_cast<I>(numer / denom);
        auto const rT = static_cast<I>(numer % denom);
        assert(numer == static_cast<I>(static_cast<I>(denom * qT) + rT));
        // Asked only where a counterpart exists to say it in, |MIN| fitting in no other type.
        if constexpr (has_unsigned_counterpart<I>) {
                assert(xstd::unsigned_abs(rT) < xstd::unsigned_abs(denom));
        }
        assert(xstd::sign(rT) == xstd::sign(numer) or rT == static_cast<I>(0));
        return {.quot = qT, .rem = rT};
}

// Deleted: bool is integral-like, and dividing two of them would answer a bool quotient.
auto div(bool, bool) -> div_t<bool> = delete;

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_HPP
