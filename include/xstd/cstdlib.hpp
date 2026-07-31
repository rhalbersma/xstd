//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_HPP
#define XSTD_CSTDLIB_HPP

#include <cassert> // assert
#include <cstdint> // intmax_t, uintmax_t
#include <format>  // format, formatter
#include <limits>  // numeric_limits
#include <ostream> // ostream
#include <tuple>   // tie, tuple

namespace xstd {

// constexpr versions of <cstdlib>'s abs/labs/llabs and <cinttypes>'s imaxabs
// (P0533), with the same non-template, signed-only signatures as those
// headers themselves.
[[nodiscard]] constexpr int abs(int x) noexcept
{
        assert(x != std::numeric_limits<int>::min()); // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr long labs(long x) noexcept
{
        assert(x != std::numeric_limits<long>::min()); // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr long long llabs(long long x) noexcept
{
        assert(x != std::numeric_limits<long long>::min()); // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr std::intmax_t imaxabs(std::intmax_t x) noexcept
{
        assert(x != std::numeric_limits<std::intmax_t>::min()); // -x would overflow
        return x < 0 ? -x : x;
}

// The total counterpart of the abs family above: same |x|, but returning the
// unsigned type, so the one input abs has to exclude - the most negative
// value, whose magnitude is one past the signed maximum - is in contract
// here. Negation is done by unsigned wraparound, which is well-defined,
// rather than by -x on a signed MIN, which is not, or by widening to a
// bigger signed type, which has none to widen to at the intmax_t end.
// Same four widths and the same non-template style as abs, and likewise
// distinct names rather than an overload set: long and intmax_t are the same
// type on some platforms (e.g. 64-bit Linux) and distinct on others, so
// overloads could collide there.
[[nodiscard]] constexpr unsigned uabs(int x) noexcept
{
        return x < 0 ? static_cast<unsigned>(0) - static_cast<unsigned>(x) : static_cast<unsigned>(x);
}

[[nodiscard]] constexpr unsigned long ulabs(long x) noexcept
{
        return x < 0 ? static_cast<unsigned long>(0) - static_cast<unsigned long>(x) : static_cast<unsigned long>(x);
}

[[nodiscard]] constexpr unsigned long long ullabs(long long x) noexcept
{
        return x < 0 ? static_cast<unsigned long long>(0) - static_cast<unsigned long long>(x) : static_cast<unsigned long long>(x);
}

[[nodiscard]] constexpr std::uintmax_t uimaxabs(std::intmax_t x) noexcept
{
        return x < 0 ? static_cast<std::uintmax_t>(0) - static_cast<std::uintmax_t>(x) : static_cast<std::uintmax_t>(x);
}

// not part of <cstdlib>, but kept to the same style and the same four
// widths as the abs family above: plain integral types, no templates.
[[nodiscard]] constexpr int sign(int x) noexcept
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

[[nodiscard]] constexpr int lsign(long x) noexcept
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

[[nodiscard]] constexpr int llsign(long long x) noexcept
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

[[nodiscard]] constexpr int imaxsign(std::intmax_t x) noexcept
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

struct div_t
{
        int quot, rem;
        [[nodiscard]] friend constexpr bool operator==(div_t const&, div_t const&) noexcept = default;
};

struct ldiv_t
{
        long quot, rem;
        [[nodiscard]] friend constexpr bool operator==(ldiv_t const&, ldiv_t const&) noexcept = default;
};

struct lldiv_t
{
        long long quot, rem;
        [[nodiscard]] friend constexpr bool operator==(lldiv_t const&, lldiv_t const&) noexcept = default;
};

struct imaxdiv_t
{
        std::intmax_t quot, rem;
        [[nodiscard]] friend constexpr bool operator==(imaxdiv_t const&, imaxdiv_t const&) noexcept = default;
};

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// constexpr versions of <cstdlib>'s div/ldiv/lldiv and <cinttypes>'s
// imaxdiv. The back-multiplication self-check (numer == denom * q + r,
// verified in a type wide enough that the multiplication itself can't
// overflow) is only possible for div: long long is guaranteed wide enough
// to hold the product of two ints, but there is no portable type wider than
// long long/intmax_t to give the same guarantee for the other three, so
// those rely solely on the sign and magnitude assertions below.
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
[[nodiscard]] constexpr div_t div(int numer, int denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<int>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(static_cast<long long>(numer) == (static_cast<long long>(denom) * qT) + rT);
        assert(uabs(rT) < uabs(denom));
        assert(sign(rT) == sign(numer) || rT == 0);
        return {.quot = qT, .rem = rT};
}

[[nodiscard]] constexpr ldiv_t ldiv(long numer, long denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<long>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(ulabs(rT) < ulabs(denom));
        assert(lsign(rT) == lsign(numer) || rT == 0);
        return {.quot = qT, .rem = rT};
}

[[nodiscard]] constexpr lldiv_t lldiv(long long numer, long long denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<long long>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(ullabs(rT) < ullabs(denom));
        assert(llsign(rT) == llsign(numer) || rT == 0);
        return {.quot = qT, .rem = rT};
}

[[nodiscard]] constexpr imaxdiv_t imaxdiv(std::intmax_t numer, std::intmax_t denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<std::intmax_t>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(uimaxabs(rT) < uimaxabs(denom));
        assert(imaxsign(rT) == imaxsign(numer) || rT == 0);
        return {.quot = qT, .rem = rT};
}

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
[[nodiscard]] constexpr div_t euclidean_div(int numer, int denom) noexcept
{
        assert(denom != 0);
        auto const divT = div(numer, denom);
        auto const I = divT.rem >= 0 ? 0 : (denom > 0 ? 1 : -1);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + (I * denom);
        assert(static_cast<long long>(numer) == (static_cast<long long>(denom) * qE) + rE);
        assert(uabs(rE) < uabs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

[[nodiscard]] constexpr ldiv_t euclidean_ldiv(long numer, long denom) noexcept
{
        assert(denom != 0);
        auto const divT = ldiv(numer, denom);
        auto const I = divT.rem >= 0 ? 0L : (denom > 0 ? 1L : -1L);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + (I * denom);
        assert(ulabs(rE) < ulabs(denom));
        assert(lsign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

[[nodiscard]] constexpr lldiv_t euclidean_lldiv(long long numer, long long denom) noexcept
{
        assert(denom != 0);
        auto const divT = lldiv(numer, denom);
        auto const I = divT.rem >= 0 ? 0LL : (denom > 0 ? 1LL : -1LL);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + (I * denom);
        assert(ullabs(rE) < ullabs(denom));
        assert(llsign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

[[nodiscard]] constexpr imaxdiv_t euclidean_imaxdiv(std::intmax_t numer, std::intmax_t denom) noexcept
{
        assert(denom != 0);
        auto const divT = imaxdiv(numer, denom);
        auto const I = divT.rem >= 0 ? std::intmax_t{0} : (denom > 0 ? std::intmax_t{1} : std::intmax_t{-1});
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + (I * denom);
        assert(uimaxabs(rE) < uimaxabs(denom));
        assert(imaxsign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
[[nodiscard]] constexpr div_t floored_div(int numer, int denom) noexcept
{
        assert(denom != 0);
        auto const divT = div(numer, denom);
        auto const I = sign(divT.rem) == -sign(denom) ? 1 : 0;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + (I * denom);
        assert(static_cast<long long>(numer) == (static_cast<long long>(denom) * qF) + rF);
        assert(uabs(rF) < uabs(denom));
        assert(rF == 0 || sign(rF) == sign(denom));
        return {.quot = qF, .rem = rF};
}

[[nodiscard]] constexpr ldiv_t floored_ldiv(long numer, long denom) noexcept
{
        assert(denom != 0);
        auto const divT = ldiv(numer, denom);
        auto const I = lsign(divT.rem) == -lsign(denom) ? 1L : 0L;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + (I * denom);
        assert(ulabs(rF) < ulabs(denom));
        assert(rF == 0 || lsign(rF) == lsign(denom));
        return {.quot = qF, .rem = rF};
}

[[nodiscard]] constexpr lldiv_t floored_lldiv(long long numer, long long denom) noexcept
{
        assert(denom != 0);
        auto const divT = lldiv(numer, denom);
        auto const I = llsign(divT.rem) == -llsign(denom) ? 1LL : 0LL;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + (I * denom);
        assert(ullabs(rF) < ullabs(denom));
        assert(rF == 0 || llsign(rF) == llsign(denom));
        return {.quot = qF, .rem = rF};
}

[[nodiscard]] constexpr imaxdiv_t floored_imaxdiv(std::intmax_t numer, std::intmax_t denom) noexcept
{
        assert(denom != 0);
        auto const divT = imaxdiv(numer, denom);
        auto const I = imaxsign(divT.rem) == -imaxsign(denom) ? std::intmax_t{1} : std::intmax_t{0};
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + (I * denom);
        assert(uimaxabs(rF) < uimaxabs(denom));
        assert(rF == 0 || imaxsign(rF) == imaxsign(denom));
        return {.quot = qF, .rem = rF};
}

} // namespace xstd

// Specialized via qualified-id (template<> struct std::formatter<...>)
// rather than inside a reopened "namespace std { ... }" block: both forms
// are equally legal here (the standard explicitly permits specializing
// std::formatter for program-defined types), but the qualified form avoids
// clang-tidy's bugprone-std-namespace-modification finding, which otherwise
// flags any reopening of namespace std regardless of what's inside it.
template<>
struct std::formatter<xstd::div_t> : std::formatter<std::tuple<int const&, int const&>>
{
        auto format(xstd::div_t const& d, auto& ctx) const
        {
                return std::formatter<std::tuple<int const&, int const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

template<>
struct std::formatter<xstd::ldiv_t> : std::formatter<std::tuple<long const&, long const&>>
{
        auto format(xstd::ldiv_t const& d, auto& ctx) const
        {
                return std::formatter<std::tuple<long const&, long const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

template<>
struct std::formatter<xstd::lldiv_t> : std::formatter<std::tuple<long long const&, long long const&>>
{
        auto format(xstd::lldiv_t const& d, auto& ctx) const
        {
                return std::formatter<std::tuple<long long const&, long long const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

template<>
struct std::formatter<xstd::imaxdiv_t> : std::formatter<std::tuple<std::intmax_t const&, std::intmax_t const&>>
{
        auto format(xstd::imaxdiv_t const& d, auto& ctx) const
        {
                return std::formatter<std::tuple<std::intmax_t const&, std::intmax_t const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

namespace xstd {

// narrow std::ostream only, not the full basic_ostream<charT, traits>
// generality (no wide-character support): these exist solely so
// Boost.Test can print div_t/ldiv_t/lldiv_t/imaxdiv_t values in test
// diagnostics. Application code should format these types via
// std::format/std::print directly rather than through operator<<.
inline auto& operator<<(std::ostream& ostr, div_t const& d)
{
        return ostr << std::format("{}", d);
}

inline auto& operator<<(std::ostream& ostr, ldiv_t const& d)
{
        return ostr << std::format("{}", d);
}

inline auto& operator<<(std::ostream& ostr, lldiv_t const& d)
{
        return ostr << std::format("{}", d);
}

inline auto& operator<<(std::ostream& ostr, imaxdiv_t const& d)
{
        return ostr << std::format("{}", d);
}

} // namespace xstd

#endif // XSTD_CSTDLIB_HPP
