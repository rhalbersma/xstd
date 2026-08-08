//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_HPP
#define XSTD_CSTDLIB_DIV_HPP

#include <xstd/concepts/nothrow_arithmetic.hpp>   // nothrow_arithmetic
#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdlib/div_t.hpp>                 // IWYU pragma: export; div_t
#include <xstd/cstdlib/sign.hpp>                  // sign
#include <xstd/cstdlib/unsigned_abs.hpp>          // unsigned_abs
#include <cassert>                                // assert
#include <limits>                                 // numeric_limits

namespace xstd {

// Truncated division, as specified by [expr.mul].
template<signed_integral_like S>
[[nodiscard]] constexpr auto div(S numer, S denom) noexcept(nothrow_arithmetic<S>)
        -> div_t<S>
{
        assert(denom != static_cast<S>(0));
        assert(numer != std::numeric_limits<S>::min() or denom != static_cast<S>(-1));
        auto const qT = static_cast<S>(numer / denom);
        auto const rT = static_cast<S>(numer % denom);
        assert(numer == static_cast<S>(static_cast<S>(denom * qT) + rT));
        assert(unsigned_abs(rT) < unsigned_abs(denom));
        assert(sign(rT) == sign(numer) or rT == static_cast<S>(0));
        return {.quot = qT, .rem = rT};
}

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_HPP
