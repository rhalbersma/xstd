//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_T_HPP
#define XSTD_CSTDLIB_DIV_T_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <format>                                 // format, formatter
#include <ostream>                                // ostream
#include <tuple>                                  // tie, tuple

namespace xstd {

template<signed_integral_like T>
struct div_t
{
        T quot, rem;
        [[nodiscard]] friend constexpr auto operator==(div_t const&, div_t const&) noexcept -> bool = default;

        // Narrow stream support for test diagnostics.
        friend auto operator<<(std::ostream& ostr, div_t const& d)
                -> std::ostream&
        {
                return ostr << std::format("{}", d);
        }
};

// Explicit to keep -Wctad-maybe-unsupported quiet.
template<signed_integral_like T>
div_t(T, T) -> div_t<T>;

} // namespace xstd

// P3391 makes the delegated tuple formatter constexpr where supported.
#ifdef __cpp_lib_constexpr_format
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

template<class T>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::div_t<T>> : std::formatter<std::tuple<T const&, T const&>>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<T> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                return std::formatter<std::tuple<T const&, T const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_CSTDLIB_DIV_T_HPP
