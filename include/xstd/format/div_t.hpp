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

// A div_t is formatted as "(quot, rem)" by two partial specializations of
// std::formatter, one per way of producing that: the tuple formatter where the
// standard library can format a tuple of S, and xstd::to_chars where it cannot.
// Both spellings produce the same characters, so which one runs is not
// observable in the output - only in the spec grammar parse() accepts, and in
// whether div_t formats at all: tuple formatting (p2286) requires the element
// type to be formattable, and an integer-class type need not be, so the
// Microsoft STL's 128-bit classes would have no rendering without the second.
// xstd::to_chars asks nothing of S beyond what signed_integral_like already
// guarantees, so div_t formats for every type div_t accepts, on every
// implementation.
//
// Two specializations rather than one with a conditional base and an if
// constexpr in format(): each one then names its own base outright, and each
// format() is written against that base alone. Which one a div_t selects is
// left to partial ordering, on the constraints - the tuple one requires
// std::formattable of the base it inherits on top of what both require of S,
// and a conjunction subsumes its left operand, so the two are equally
// specialized on their arguments and the more constrained one wins wherever it
// matches at all. Spelling the second's constraint as the negation would work
// too, but a negated atomic constraint does not subsume, so exclusivity and
// exhaustiveness would become an invariant to maintain across two edits rather
// than a property of the constraints.
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
// these constexpr regardless would be ill-formed, no diagnostic required, so
// both compilers would accept it in silence. P3391 (constexpr std::format),
// plenary-approved for C++29, makes the standard library's own formatter
// specializations constexpr and announces that with __cpp_lib_constexpr_format.
// A user-defined formatter still has to opt in on its own, since a format call
// is a constant expression only if every specialization it uses is
// constexpr-enabled. The macro below is that opt-in, spelled conditionally so
// the keyword appears exactly where the base can honor it. It is #undef'd after
// its last use: it is a detail of these member functions rather than part of
// xstd's interface, and macro replacement happens long before they are
// instantiated. Everything it reaches is already constexpr - both xstd::to_chars
// and std::basic_string's transient allocation are.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

// The one every div_t matches, which renders the members itself. Inheriting the
// string formatter is what carries fill, alignment and width; the grammar it
// accepts is that base's, so precision is part of it too.
//
// S is spelled signed_integral_like here and below, though nothing turns on it:
// S is deduced from div_t<S>, which is itself constrained, so a div_t whose
// element type does not satisfy this cannot be named in the first place and the
// constraint can never be the reason a specialization fails to match. It is
// written out because the alternative reads as though div_t<S> were open to any
// S, and because the bodies do rely on it - to_chars_max_size and to_chars ask
// for integral_like, which is the half of this concept they need.
template<xstd::signed_integral_like S, class CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::basic_string_view<CharT>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                using base = std::formatter<std::basic_string_view<CharT>, CharT>;

                constexpr auto N = xstd::to_chars_max_size<S>;
                auto widened = std::basic_string<CharT>{};
                auto buffer = std::array<char, N>{};

                auto const append = [&](S const value) XSTD_CONSTEXPR_FORMAT -> void {
                        auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                        // The buffer is sized for base 2, so decimal always fits.
                        assert(result.ec == std::errc{});
                        for (auto const* p = buffer.data(); p != result.ptr; ++p) {
                                widened.push_back(static_cast<CharT>(*p));
                        }
                };

                // Spelled to match what the tuple formatter produces, so the
                // two specializations render a div_t identically.
                widened.push_back(static_cast<CharT>('('));
                append(d.quot);
                widened.push_back(static_cast<CharT>(','));
                widened.push_back(static_cast<CharT>(' '));
                append(d.rem);
                widened.push_back(static_cast<CharT>(')'));

                return base::format(widened, ctx);
        }
};

// The more constrained one. div_t is tuple-like, so where the standard library
// can format the pair directly that is the rendering to use: it is the
// standard's own, it needs no intermediate string, and it keeps whatever
// tuple-specific spec handling the implementation offers - the tuple specs n
// and m, where the base above takes precision.
//
// The constraint is std::formattable of the base type itself, spelled out
// rather than routed through a concept of xstd's own: the requirement is
// exactly that the base below is usable. Note that it asks after the *tuple*,
// not after S. One predicate then covers both of the ways this rendering can be
// unavailable, which are not the same way and do not coincide:
//
// - S is not formattable. The Microsoft STL's 128-bit classes are the case
//   that matters; nothing in that implementation formats them.
// - Tuple formatting itself is missing. p2286 reached libstdc++ in GCC 15, so
//   on an older one formattable<tuple<int const&, int const&>> is false even
//   though int is perfectly formattable, and a condition on S alone would
//   inherit a formatter that does not exist.
template<xstd::signed_integral_like S, class CharT>
        requires std::formattable<std::tuple<S const&, S const&>, CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_t<S>, CharT> : std::formatter<std::tuple<S const&, S const&>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<S> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                using base = std::formatter<std::tuple<S const&, S const&>, CharT>;

                // tie yields exactly tuple<S const&, S const&>, the base's own
                // type, so nothing is copied on the way in.
                return base::format(std::tie(d.quot, d.rem), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_FORMAT_DIV_T_HPP
