//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP
#define XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP

#include <xstd/cstdlib/div.hpp>  // div
#include <xstd/cstdlib/sign.hpp> // sign
#include <xstd/cstdlib/uabs.hpp> // uabs
#include <cassert>               // assert

namespace xstd {

// Euclidean division: the remainder is nonnegative.
template<signed_integral_like T>
[[nodiscard]] constexpr auto euclidean_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<T>(0);
        auto const one = static_cast<T>(1);
        auto const adjust = rT < zero;
        auto const qE = adjust ? (denom > zero ? static_cast<T>(qT - one) : static_cast<T>(qT + one)) : qT;
        auto const rE = adjust ? (denom > zero ? static_cast<T>(rT + denom) : static_cast<T>(rT - denom)) : rT;
        assert(uabs(rE) < uabs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

} // namespace xstd

#endif // XSTD_CSTDLIB_EUCLIDEAN_DIV_HPP
