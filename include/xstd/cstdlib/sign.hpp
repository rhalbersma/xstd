//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_SIGN_HPP
#define XSTD_CSTDLIB_SIGN_HPP

#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/type_traits/is_unsigned_like.hpp>        // is_unsigned_like_v

namespace xstd {

// Returns -1, 0, or 1; over an unsigned type, 0 or 1.
template<integral_like I>
[[nodiscard]] constexpr auto sign(I x) noexcept(nothrow_integral_operators<I>)
        -> int
{
        auto const zero = static_cast<I>(0);
        // One comparison rather than two. "x < zero" is false for every value
        // of an unsigned type rather than ill-formed, so the difference would
        // have been correct as written - but it is a comparison the answer can
        // never depend on, and for an integer-class type a comparison is a
        // call rather than an instruction. This function is on the path of
        // div's postconditions and floored_div's adjustment, so that is a call
        // per use of either.
        if constexpr (is_unsigned_like_v<I>) {
                return static_cast<int>(zero < x);
        } else {
                return static_cast<int>(zero < x) - static_cast<int>(x < zero);
        }
}

// Deleted as <xstd/charconv/to_chars.hpp> deletes it, and for its reason:
// bool is integral-like, and sign(true) would answer 1.
auto sign(bool) -> int = delete;

} // namespace xstd

#endif // XSTD_CSTDLIB_SIGN_HPP
