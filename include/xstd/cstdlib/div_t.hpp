//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_T_HPP
#define XSTD_CSTDLIB_DIV_T_HPP

#include <xstd/concepts/integer_like.hpp> // integer_like

namespace xstd {

template<integer_like I>
struct div_t
{
        I quot, rem;
        // Both specifiers are implicit for a defaulted function; [[nodiscard]] is not.
        [[nodiscard]] friend auto operator==(div_t const&, div_t const&) -> bool = default;
};

// Explicit to keep -Wctad-maybe-unsupported quiet.
template<integer_like I>
div_t(I, I) -> div_t<I>;

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_T_HPP
