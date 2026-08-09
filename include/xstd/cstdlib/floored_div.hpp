//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_FLOORED_DIV_HPP
#define XSTD_CSTDLIB_FLOORED_DIV_HPP

#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/concepts/signed_integral_like.hpp>       // signed_integral_like
#include <xstd/cstdlib/div.hpp>                         // div
#include <xstd/cstdlib/div_t.hpp>                       // IWYU pragma: export; div_t
#include <xstd/cstdlib/sign.hpp>                        // sign
#include <xstd/cstdlib/unsigned_abs.hpp>                // unsigned_abs
#include <cassert>                                      // assert

namespace xstd {

// Floored division: the remainder has the denominator's sign.
template<signed_integral_like S>
[[nodiscard]] constexpr auto floored_div(S numer, S denom) noexcept(nothrow_integral_operators<S>)
        -> div_t<S>
{
        assert(denom != static_cast<S>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<S>(0);
        auto const one = static_cast<S>(1);
        auto const adjust = sign(rT) == -sign(denom);
        auto const qF = static_cast<S>(qT - (adjust ? one : zero));
        auto const rF = static_cast<S>(rT + (adjust ? denom : zero));
        assert(unsigned_abs(rF) < unsigned_abs(denom));
        assert(rF == static_cast<S>(0) or sign(rF) == sign(denom));
        return {.quot = qF, .rem = rF};
}

} // namespace xstd

#endif // XSTD_CSTDLIB_FLOORED_DIV_HPP
