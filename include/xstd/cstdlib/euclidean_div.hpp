//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP
#define XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdlib/div.hpp>                   // div
#include <xstd/cstdlib/div_t.hpp>                 // IWYU pragma: export; div_t
#include <xstd/cstdlib/sign.hpp>                  // sign
#include <xstd/cstdlib/unsigned_abs.hpp>          // unsigned_abs
#include <cassert>                                // assert

namespace xstd {

// Euclidean division: the remainder is nonnegative.
template<signed_integral_like S>
[[nodiscard]] constexpr auto euclidean_div(S numer, S denom) noexcept
        -> div_t<S>
{
        assert(denom != static_cast<S>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<S>(0);
        auto const one = static_cast<S>(1);
        auto const adjust = rT < zero;
        auto const qE = adjust ? (denom > zero ? static_cast<S>(qT - one) : static_cast<S>(qT + one)) : qT;
        auto const rE = adjust ? (denom > zero ? static_cast<S>(rT + denom) : static_cast<S>(rT - denom)) : rT;
        assert(unsigned_abs(rE) < unsigned_abs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

} // namespace xstd

#endif // XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP
