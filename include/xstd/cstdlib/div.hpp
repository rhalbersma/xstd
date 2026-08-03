//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_HPP
#define XSTD_CSTDLIB_DIV_HPP

#include <xstd/cstdlib/div_t.hpp> // div_t
#include <xstd/cstdlib/sign.hpp>  // sign
#include <xstd/cstdlib/uabs.hpp>  // uabs
#include <cassert>                // assert
#include <limits>                 // numeric_limits

namespace xstd {

// Truncated division, as specified by [expr.mul].
template<signed_integral_like T>
[[nodiscard]] constexpr auto div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        assert(numer != std::numeric_limits<T>::min() or denom != static_cast<T>(-1));
        auto const qT = static_cast<T>(numer / denom);
        auto const rT = static_cast<T>(numer % denom);
        assert(numer == static_cast<T>(static_cast<T>(denom * qT) + rT));
        assert(uabs(rT) < uabs(denom));
        assert(sign(rT) == sign(numer) or rT == static_cast<T>(0));
        return {.quot = qT, .rem = rT};
}

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_HPP
