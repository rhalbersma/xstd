//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_DIV_T_HPP
#define XSTD_FORMAT_DIV_T_HPP

#include <xstd/charconv/to_chars.hpp>             // to_chars, to_chars_max_size
#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdlib/div_t.hpp>                 // div_t
#include <array>                                  // array
#include <cassert>                                // assert
#include <format>                                 // formatter
#include <string>                                 // basic_string
#include <string_view>                            // basic_string_view
#include <system_error>                           // errc
#include <tuple>                                  // tie, tuple

// A div_t renders as "(quot, rem)": by the tuple formatter where the standard
// library can, and by xstd::to_chars where it cannot.

// The base's format() is not constexpr before P3391 (C++29), and marking these
// constexpr anyway would be ill-formed with no diagnostic required.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

// The one every div_t matches, rendering the members itself; the string base
// carries fill, alignment and width, and its grammar is what parse() accepts.
template<xstd::signed_integral_like S, class CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::basic_string_view<CharT>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                constexpr auto N = xstd::to_chars_max_size<S>;
                auto widened = std::basic_string<CharT>{};
                auto buffer = std::array<char, N>{};

                // The string is a parameter rather than a capture, so that
                // nothing holds a reference to it across the push_back calls
                // below. Clang's -Wlifetime-safety-invalidation reads a
                // captured container as borrowed for the lifetime of the
                // lambda, and push_back as ending that borrow - true of a
                // reference to an element, which this is not, but the analysis
                // cannot tell the two apart.
                auto const append = [&buffer](std::basic_string<CharT>& out, S const value) XSTD_CONSTEXPR_FORMAT -> void {
                        auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                        // The buffer is sized for base 2, so decimal always fits.
                        assert(result.ec == std::errc{});
                        for (auto const* p = buffer.data(); p != result.ptr; ++p) {
                                out.push_back(static_cast<CharT>(*p));
                        }
                };

                // Spelled to match what the tuple formatter produces, so the
                // two specializations render a div_t identically.
                widened.push_back(static_cast<CharT>('('));
                append(widened, d.quot);
                widened.push_back(static_cast<CharT>(','));
                widened.push_back(static_cast<CharT>(' '));
                append(widened, d.rem);
                widened.push_back(static_cast<CharT>(')'));

                return std::formatter<std::basic_string_view<CharT>, CharT>::format(widened, ctx);
        }
};

// The more constrained one: the standard's own rendering. The constraint asks
// after the tuple rather than after S, which covers both ways it can be absent.
template<xstd::signed_integral_like S, class CharT>
        requires std::formattable<std::tuple<S const&, S const&>, CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::tuple<S const&, S const&>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                // tie yields exactly tuple<S const&, S const&>, the base's own
                // type, so nothing is copied on the way in.
                return std::formatter<std::tuple<S const&, S const&>, CharT>::format(std::tie(d.quot, d.rem), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_FORMAT_DIV_T_HPP
