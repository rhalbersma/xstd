//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_ABS_HPP
#define XSTD_CSTDLIB_ABS_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <cassert>                                // assert
#include <limits>                                 // numeric_limits

namespace xstd {

// constexpr generalization of abs/labs/llabs/imaxabs.
template<signed_integral_like T>
[[nodiscard]] constexpr auto abs(T x) noexcept
        -> T
{
        auto const zero = static_cast<T>(0);
        assert(x != std::numeric_limits<T>::min()); // -x would overflow
        return static_cast<T>(x < zero ? -x : x);
}

} // namespace xstd

#endif // XSTD_CSTDLIB_ABS_HPP
