//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_HPP
#define XSTD_FORMAT_HPP

#include <xstd/cstdlib/div_t.hpp> // div_t
#include <format>                 // formatter
#include <tuple>                  // tie, tuple

// P3391 makes the delegated tuple formatter constexpr where supported.
#ifdef __cpp_lib_constexpr_format
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

template<class S>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::div_t<S>> : std::formatter<std::tuple<S const&, S const&>>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                return std::formatter<std::tuple<S const&, S const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_FORMAT_HPP
