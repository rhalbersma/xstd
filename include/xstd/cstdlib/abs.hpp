//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_ABS_HPP
#define XSTD_CSTDLIB_ABS_HPP

#include <xstd/concepts/exposition_only.hpp>      // nothrow_arithmetic
#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <cassert>                                // assert
#include <limits>                                 // numeric_limits

namespace xstd {

// constexpr generalization of abs/labs/llabs/imaxabs.
template<signed_integral_like S>
[[nodiscard]] constexpr auto abs(S x) noexcept(exposition_only::nothrow_arithmetic<S>)
        -> S
{
        auto const zero = static_cast<S>(0);
        assert(x != std::numeric_limits<S>::min()); // -x would overflow
        return static_cast<S>(x < zero ? -x : x);
}

} // namespace xstd

#endif // XSTD_CSTDLIB_ABS_HPP
