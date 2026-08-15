//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_UNSIGNED_ABS_HPP
#define XSTD_CSTDLIB_UNSIGNED_ABS_HPP

#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/type_traits/is_unsigned_like.hpp>        // is_unsigned_like_v
#include <xstd/type_traits/make_unsigned_like.hpp>      // make_unsigned_like_t

namespace xstd {

// Like Rust's unsigned_abs: total absolute value, returning the unsigned
// counterpart so MIN is valid. Shares abs's shape, which is what says the two
// are the same function over an unsigned type and differ only in return type;
// over a signed one this is the total half of the pair and abs the partial.
template<integral_like I>
[[nodiscard]] constexpr auto unsigned_abs(I x) noexcept(nothrow_integral_operators<I>)
{
        if constexpr (is_unsigned_like_v<I>) {
                return x;
        } else {
                using U = make_unsigned_like_t<I>;
                auto const zero = static_cast<U>(0);
                auto const u = static_cast<U>(x);
                return static_cast<U>(x < static_cast<I>(0) ? zero - u : u);
        }
}

// Deleted rather than left to fail inside the body. bool satisfies
// integral_like, make_unsigned_like<bool> is the empty primary, and the alias
// is formed where a substitution failure can no longer be one - so without
// this the call is ill-formed instead of unsatisfied.
auto unsigned_abs(bool) -> bool = delete;

} // namespace xstd

#endif // XSTD_CSTDLIB_UNSIGNED_ABS_HPP
