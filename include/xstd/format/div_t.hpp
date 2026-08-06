//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_FORMAT_DIV_T_HPP
#define XSTD_FORMAT_DIV_T_HPP

#include <xstd/charconv/to_chars.hpp> // to_chars, to_chars_max_size
#include <xstd/cstdlib/div_t.hpp>     // div_t
#include <array>                      // array
#include <cassert>                    // assert
#include <format>                     // formatter
#include <string>                     // basic_string
#include <string_view>                // basic_string_view
#include <system_error>               // errc

// This formatter renders the two members through xstd::to_chars rather than
// delegating to the tuple formatter. Tuple formatting (p2286) requires the
// element type to be formattable, and an integer-class type need not be: the
// Microsoft STL's own 128-bit classes have no formatter at all. xstd::to_chars
// asks nothing of S beyond what signed_integral_like already guarantees, so
// div_t formats for every type div_t accepts.
//
// Nothing here specializes std::formatter for a 128-bit type. Those are either
// built-ins or standard-library types, so specializing for them is outside what
// [namespace.std]/2 permits, and where the standard library already provides
// one - libstdc++ does for __int128, precisely under __STRICT_ANSI__ - it would
// displace a formatter that handles the whole spec grammar with one that does
// not. div_t is program-defined, so its formatter is xstd's to write.
//
// Formatting is the one xstd operation that is not usable at compile time, and
// not by its own choice: the base's format() is not constexpr in C++23. Marking
// this one constexpr regardless would be ill-formed, no diagnostic required, so
// both compilers would accept it in silence. P3391 (constexpr std::format),
// plenary-approved for C++29, makes the standard library's own formatter
// specializations constexpr and announces that with __cpp_lib_constexpr_format.
// A user-defined formatter still has to opt in on its own, since a format call
// is a constant expression only if every specialization it uses is
// constexpr-enabled. The macro below is that opt-in, spelled conditionally so
// the keyword appears exactly where the base can honor it. It is #undef'd right
// after its single use: it is a detail of this one member function rather than
// part of xstd's interface, and macro replacement happens long before the
// member is instantiated. Everything it reaches is already constexpr - both
// xstd::to_chars and std::basic_string's transient allocation are.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

template<class S, class CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::basic_string_view<CharT>, CharT>
{
        // Inheriting the base's parse() is what keeps fill, alignment and width
        // working on the rendered "(quot, rem)" as a whole.
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                constexpr auto N = xstd::to_chars_max_size<S>;
                auto widened = std::basic_string<CharT>{};
                auto buffer = std::array<char, N>{};

                auto const append = [&](S const value) XSTD_CONSTEXPR_FORMAT {
                        auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                        // The buffer is sized for base 2, so decimal always fits.
                        assert(result.ec == std::errc{});
                        for (auto const* p = buffer.data(); p != result.ptr; ++p) {
                                widened.push_back(static_cast<CharT>(*p));
                        }
                };

                widened.push_back(static_cast<CharT>('('));
                append(d.quot);
                widened.push_back(static_cast<CharT>(','));
                widened.push_back(static_cast<CharT>(' '));
                append(d.rem);
                widened.push_back(static_cast<CharT>(')'));

                return std::formatter<std::basic_string_view<CharT>, CharT>::format(widened, ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_FORMAT_DIV_T_HPP
