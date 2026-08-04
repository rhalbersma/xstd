//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_DIV_T_HPP
#define XSTD_FORMAT_DIV_T_HPP

#include <xstd/cstdlib/div_t.hpp> // div_t
#include <xstd/format/int128.hpp> // IWYU pragma: keep; detail::to_decimal
#include <format>                 // formatter
#include <string>                 // basic_string, string
#include <string_view>            // basic_string_view

namespace xstd::detail {

template<class CharT>
[[nodiscard]] auto widen(std::string const& str)
        -> std::basic_string<CharT>
{
        auto result = std::basic_string<CharT>{};
        result.reserve(str.size());
        for (auto const ch : str) {
                result.push_back(static_cast<CharT>(ch));
        }
        return result;
}

} // namespace xstd::detail

template<class S, class CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::basic_string_view<CharT>, CharT>
{
        [[nodiscard]] auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                auto decimal = std::string{"("};
                decimal += xstd::detail::to_decimal(d.quot);
                decimal += ", ";
                decimal += xstd::detail::to_decimal(d.rem);
                decimal += ")";

                auto const widened = xstd::detail::widen<CharT>(decimal);
                return std::formatter<std::basic_string_view<CharT>, CharT>::format(widened, ctx);
        }
};

#endif // XSTD_FORMAT_DIV_T_HPP
