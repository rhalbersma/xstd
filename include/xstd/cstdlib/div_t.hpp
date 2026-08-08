//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_T_HPP
#define XSTD_CSTDLIB_DIV_T_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like

namespace xstd {

template<signed_integral_like S>
struct div_t
{
        S quot, rem;
        [[nodiscard]] friend constexpr auto operator==(div_t const&, div_t const&) -> bool = default;
};

// Explicit to keep -Wctad-maybe-unsupported quiet.
template<signed_integral_like S>
div_t(S, S) -> div_t<S>;

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_T_HPP
