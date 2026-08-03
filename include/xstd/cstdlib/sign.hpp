//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_SIGN_HPP
#define XSTD_CSTDLIB_SIGN_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like

namespace xstd {

// Returns -1, 0, or 1.
template<signed_integral_like S>
[[nodiscard]] constexpr auto sign(S x) noexcept
        -> int
{
        auto const zero = static_cast<S>(0);
        return static_cast<int>(zero < x) - static_cast<int>(x < zero);
}

} // namespace xstd

#endif // XSTD_CSTDLIB_SIGN_HPP
