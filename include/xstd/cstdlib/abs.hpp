//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_ABS_HPP
#define XSTD_CSTDLIB_ABS_HPP

#include <xstd/concepts/integer_like.hpp>               // integer_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/type_traits/is_unsigned_like.hpp>        // is_unsigned_like_v
#include <cassert>                                      // assert
#include <limits>                                       // numeric_limits

namespace xstd {

// constexpr generalization of abs/labs/llabs/imaxabs.
template<integer_like I>
[[nodiscard]] constexpr auto abs(I x) noexcept(nothrow_integral_operators<I>)
        -> I
{
        // An unsigned value is its own magnitude, and its min() is 0.
        if constexpr (is_unsigned_like_v<I>) {
                return x;
        } else {
                auto const zero = static_cast<I>(0);
                assert(x != std::numeric_limits<I>::min()); // -x would overflow
                return static_cast<I>(x < zero ? -x : x);
        }
}

} // namespace xstd

#endif // XSTD_CSTDLIB_ABS_HPP
