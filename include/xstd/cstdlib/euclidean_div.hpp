//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP
#define XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP

#include <xstd/concepts/has_unsigned_counterpart.hpp>   // has_unsigned_counterpart
#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/cstdlib/div.hpp>                         // div
#include <xstd/cstdlib/div_t.hpp>                       // IWYU pragma: export; div_t
#include <xstd/cstdlib/sign.hpp>                        // sign
#include <xstd/cstdlib/unsigned_abs.hpp>                // unsigned_abs
#include <xstd/type_traits/is_unsigned_like.hpp>        // is_unsigned_like_v
#include <cassert>                                      // assert

namespace xstd {

// Euclidean division: the remainder is nonnegative.
template<integral_like I>
        requires has_unsigned_counterpart<I>
[[nodiscard]] constexpr auto euclidean_div(I numer, I denom) noexcept(nothrow_integral_operators<I>)
        -> div_t<I>
{
        assert(denom != static_cast<I>(0));
        // An unsigned truncated remainder is already nonnegative, so it is the answer.
        if constexpr (is_unsigned_like_v<I>) {
                // Qualified: unqualified, ADL finds Boost.Int128's own div and it wins.
                return xstd::div(numer, denom);
        } else {
                auto const [qT, rT] = xstd::div(numer, denom);
                auto const zero = static_cast<I>(0);
                auto const one = static_cast<I>(1);
                auto const adjust = rT < zero;
                auto const qE = adjust ? (denom > zero ? static_cast<I>(qT - one) : static_cast<I>(qT + one)) : qT;
                auto const rE = adjust ? (denom > zero ? static_cast<I>(rT + denom) : static_cast<I>(rT - denom)) : rT;
                assert(xstd::unsigned_abs(rE) < xstd::unsigned_abs(denom));
                assert(xstd::sign(rE) >= 0);
                return {.quot = qE, .rem = rE};
        }
}

// Deleted for div's reason, reached through it.
auto euclidean_div(bool, bool) -> div_t<bool> = delete;

} // namespace xstd

#endif // XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP
