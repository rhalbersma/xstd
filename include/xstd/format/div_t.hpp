//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_DIV_T_HPP
#define XSTD_FORMAT_DIV_T_HPP

#include <xstd/cstdlib/div_t.hpp> // div_t
#include <format>                 // formatter
#include <tuple>                  // tuple

template<class S, class CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::tuple<S, S>, CharT>
{
        [[nodiscard]] auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                return std::formatter<std::tuple<S, S>, CharT>::format(std::tie(d.quot, d.rem), ctx);
        }
};

#endif // XSTD_FORMAT_DIV_T_HPP
