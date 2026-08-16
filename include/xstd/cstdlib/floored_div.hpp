//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_FLOORED_DIV_HPP
#define XSTD_CSTDLIB_FLOORED_DIV_HPP

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

// Floored division: the remainder has the denominator's sign.
template<integral_like I>
        requires has_unsigned_counterpart<I>
[[nodiscard]] constexpr auto floored_div(I numer, I denom) noexcept(nothrow_integral_operators<I>)
        -> div_t<I>
{
        assert(denom != static_cast<I>(0));
        // An unsigned remainder cannot disagree in sign with its denominator.
        if constexpr (is_unsigned_like_v<I>) {
                // Qualified: unqualified, ADL finds Boost.Int128's own div and it wins.
                return xstd::div(numer, denom);
        } else {
                auto const [qT, rT] = xstd::div(numer, denom);
                auto const zero = static_cast<I>(0);
                auto const one = static_cast<I>(1);
                auto const adjust = xstd::sign(rT) == -xstd::sign(denom);
                auto const qF = static_cast<I>(qT - (adjust ? one : zero));
                auto const rF = static_cast<I>(rT + (adjust ? denom : zero));
                assert(xstd::unsigned_abs(rF) < xstd::unsigned_abs(denom));
                assert(rF == static_cast<I>(0) or xstd::sign(rF) == xstd::sign(denom));
                return {.quot = qF, .rem = rF};
        }
}

// Deleted for div's reason, reached through it.
auto floored_div(bool, bool) -> div_t<bool> = delete;

} // namespace xstd

#endif // XSTD_CSTDLIB_FLOORED_DIV_HPP
