#pragma once

//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <cstdint>      // intmax_t, uintmax_t
#include <format>       // format, formatter
#include <limits>       // numeric_limits
#include <ostream>      // ostream
#include <tuple>        // tie, tuple

namespace xstd {

// constexpr versions of <cstdlib>'s abs/labs/llabs and <cinttypes>'s imaxabs
// (P0533), with the same non-template, signed-only signatures as those
// headers themselves.
[[nodiscard]] constexpr int abs(int x) noexcept
{
        assert(x != std::numeric_limits<int>::min());     // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr long labs(long x) noexcept
{
        assert(x != std::numeric_limits<long>::min());     // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr long long llabs(long long x) noexcept
{
        assert(x != std::numeric_limits<long long>::min());     // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr std::intmax_t imaxabs(std::intmax_t x) noexcept
{
        assert(x != std::numeric_limits<std::intmax_t>::min());     // -x would overflow
        return x < 0 ? -x : x;
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
        bool operator==(div_t const&) const = default;
};

struct ldiv_t
{
        long quot, rem;
        bool operator==(ldiv_t const&) const = default;
};

struct lldiv_t
{
        long long quot, rem;
        bool operator==(lldiv_t const&) const = default;
};

struct imaxdiv_t
{
        std::intmax_t quot, rem;
        bool operator==(imaxdiv_t const&) const = default;
};

namespace detail {

// |x| computed via unsigned wraparound (well-defined, unlike -x on a signed
// MIN) rather than by widening to a bigger signed type: long and intmax_t
// are the same type on some platforms (e.g. 64-bit Linux), so a same-named
// overload set for these helpers could collide there; distinct names avoid
// that regardless of platform, matching how the public family above already
// uses distinct names (abs/labs/llabs/imaxabs) rather than overloads.

[[nodiscard]] constexpr unsigned magnitude(int x) noexcept
{
        return x < 0 ? unsigned{} - static_cast<unsigned>(x) : static_cast<unsigned>(x);
}

[[nodiscard]] constexpr unsigned long lmagnitude(long x) noexcept
{
        return x < 0 ? static_cast<unsigned long>(0) - static_cast<unsigned long>(x) : static_cast<unsigned long>(x);
}

[[nodiscard]] constexpr unsigned long long llmagnitude(long long x) noexcept
{
        return x < 0 ? static_cast<unsigned long long>(0) - static_cast<unsigned long long>(x) : static_cast<unsigned long long>(x);
}

[[nodiscard]] constexpr std::uintmax_t imaxmagnitude(std::intmax_t x) noexcept
{
        return x < 0 ? static_cast<std::uintmax_t>(0) - static_cast<std::uintmax_t>(x) : static_cast<std::uintmax_t>(x);
}

}       // namespace detail

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// constexpr versions of <cstdlib>'s div/ldiv/lldiv and <cinttypes>'s
// imaxdiv. The back-multiplication self-check (numer == denom * q + r,
// verified in a type wide enough that the multiplication itself can't
// overflow) is only possible for div: long long is guaranteed wide enough
// to hold the product of two ints, but there is no portable type wider than
// long long/intmax_t to give the same guarantee for the other three, so
// those rely solely on the sign/magnitude assertions below.
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
        assert(static_cast<long long>(numer) == static_cast<long long>(denom) * qT + rT);
        assert(detail::magnitude(rT) < detail::magnitude(denom));
        assert(sign(rT) == sign(numer) || rT == 0);
        return { qT, rT };
}

[[nodiscard]] constexpr ldiv_t ldiv(long numer, long denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<long>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(detail::lmagnitude(rT) < detail::lmagnitude(denom));
        assert(lsign(rT) == lsign(numer) || rT == 0);
        return { qT, rT };
}

[[nodiscard]] constexpr lldiv_t lldiv(long long numer, long long denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<long long>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(detail::llmagnitude(rT) < detail::llmagnitude(denom));
        assert(llsign(rT) == llsign(numer) || rT == 0);
        return { qT, rT };
}

[[nodiscard]] constexpr imaxdiv_t imaxdiv(std::intmax_t numer, std::intmax_t denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<std::intmax_t>::min() && denom == -1));
        auto const qT = numer / denom;
        auto const rT = numer % denom;
        assert(detail::imaxmagnitude(rT) < detail::imaxmagnitude(denom));
        assert(imaxsign(rT) == imaxsign(numer) || rT == 0);
        return { qT, rT };
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
        auto const rE = divT.rem + I * denom;
        assert(static_cast<long long>(numer) == static_cast<long long>(denom) * qE + rE);
        assert(detail::magnitude(rE) < detail::magnitude(denom));
        assert(sign(rE) >= 0);
        return { qE, rE };
}

[[nodiscard]] constexpr ldiv_t euclidean_ldiv(long numer, long denom) noexcept
{
        assert(denom != 0);
        auto const divT = ldiv(numer, denom);
        auto const I = divT.rem >= 0 ? 0L : (denom > 0 ? 1L : -1L);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + I * denom;
        assert(detail::lmagnitude(rE) < detail::lmagnitude(denom));
        assert(lsign(rE) >= 0);
        return { qE, rE };
}

[[nodiscard]] constexpr lldiv_t euclidean_lldiv(long long numer, long long denom) noexcept
{
        assert(denom != 0);
        auto const divT = lldiv(numer, denom);
        auto const I = divT.rem >= 0 ? 0LL : (denom > 0 ? 1LL : -1LL);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + I * denom;
        assert(detail::llmagnitude(rE) < detail::llmagnitude(denom));
        assert(llsign(rE) >= 0);
        return { qE, rE };
}

[[nodiscard]] constexpr imaxdiv_t euclidean_imaxdiv(std::intmax_t numer, std::intmax_t denom) noexcept
{
        assert(denom != 0);
        auto const divT = imaxdiv(numer, denom);
        auto const I = divT.rem >= 0 ? std::intmax_t{0} : (denom > 0 ? std::intmax_t{1} : std::intmax_t{-1});
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + I * denom;
        assert(detail::imaxmagnitude(rE) < detail::imaxmagnitude(denom));
        assert(imaxsign(rE) >= 0);
        return { qE, rE };
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
        auto const rF = divT.rem + I * denom;
        assert(static_cast<long long>(numer) == static_cast<long long>(denom) * qF + rF);
        assert(detail::magnitude(rF) < detail::magnitude(denom));
        assert(rF == 0 || sign(rF) == sign(denom));
        return { qF, rF };
}

[[nodiscard]] constexpr ldiv_t floored_ldiv(long numer, long denom) noexcept
{
        assert(denom != 0);
        auto const divT = ldiv(numer, denom);
        auto const I = lsign(divT.rem) == -lsign(denom) ? 1L : 0L;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + I * denom;
        assert(detail::lmagnitude(rF) < detail::lmagnitude(denom));
        assert(rF == 0 || lsign(rF) == lsign(denom));
        return { qF, rF };
}

[[nodiscard]] constexpr lldiv_t floored_lldiv(long long numer, long long denom) noexcept
{
        assert(denom != 0);
        auto const divT = lldiv(numer, denom);
        auto const I = llsign(divT.rem) == -llsign(denom) ? 1LL : 0LL;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + I * denom;
        assert(detail::llmagnitude(rF) < detail::llmagnitude(denom));
        assert(rF == 0 || llsign(rF) == llsign(denom));
        return { qF, rF };
}

[[nodiscard]] constexpr imaxdiv_t floored_imaxdiv(std::intmax_t numer, std::intmax_t denom) noexcept
{
        assert(denom != 0);
        auto const divT = imaxdiv(numer, denom);
        auto const I = imaxsign(divT.rem) == -imaxsign(denom) ? std::intmax_t{1} : std::intmax_t{0};
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + I * denom;
        assert(detail::imaxmagnitude(rF) < detail::imaxmagnitude(denom));
        assert(rF == 0 || imaxsign(rF) == imaxsign(denom));
        return { qF, rF };
}

}       // namespace xstd

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

inline auto& operator<<(std::ostream& ostr, div_t const& d)
{
        ostr << std::format("{}", d);
        return ostr;
}

inline auto& operator<<(std::ostream& ostr, ldiv_t const& d)
{
        ostr << std::format("{}", d);
        return ostr;
}

inline auto& operator<<(std::ostream& ostr, lldiv_t const& d)
{
        ostr << std::format("{}", d);
        return ostr;
}

inline auto& operator<<(std::ostream& ostr, imaxdiv_t const& d)
{
        ostr << std::format("{}", d);
        return ostr;
}

}       // namespace xstd
