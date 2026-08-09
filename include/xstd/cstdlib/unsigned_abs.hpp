//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_UNSIGNED_ABS_HPP
#define XSTD_CSTDLIB_UNSIGNED_ABS_HPP

#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/concepts/signed_integral_like.hpp>       // signed_integral_like
#include <xstd/type_traits/make_unsigned_like.hpp>      // make_unsigned_like_t

namespace xstd {

// Like Rust's unsigned_abs: total absolute value, returning the unsigned
// counterpart so MIN is valid.
template<signed_integral_like S>
[[nodiscard]] constexpr auto unsigned_abs(S x) noexcept(nothrow_integral_operators<S>)
{
        using U = make_unsigned_like_t<S>;
        auto const zero = static_cast<U>(0);
        auto const u = static_cast<U>(x);
        return static_cast<U>(x < static_cast<S>(0) ? zero - u : u);
}

} // namespace xstd

#endif // XSTD_CSTDLIB_UNSIGNED_ABS_HPP
