#pragma once

//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <format>       // format, formatter
#include <limits>       // numeric_limits
#include <ostream>      // ostream
#include <tuple>        // tie, tuple
#include <type_traits>  // is_arithmetic_v, is_same_v

namespace xstd {

// constexpr versions of <cstdlib>'s abs/labs/llabs (P0533). Non-template and
// signed-only, mirroring <cstdlib>'s own overload set exactly.
[[nodiscard]] constexpr auto abs(int x) noexcept -> int
{
        assert(x != std::numeric_limits<int>::min());     // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr auto labs(long x) noexcept -> long
{
        assert(x != std::numeric_limits<long>::min());     // -x would overflow
        return x < 0 ? -x : x;
}

[[nodiscard]] constexpr auto llabs(long long x) noexcept -> long long
{
        assert(x != std::numeric_limits<long long>::min());     // -x would overflow
        return x < 0 ? -x : x;
}

template<class T>
[[nodiscard]] constexpr auto sign(T const& x) noexcept
        requires (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

struct div_t
{
        int quot, rem;
        bool operator==(div_t const&) const = default;
};

namespace detail {

[[nodiscard]] constexpr auto magnitude(int x) noexcept
{
        auto const y = static_cast<long long>(x);
        return y < 0 ? -y : y;
}

}       // namespace detail

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// a constexpr version of std::div(int, int)
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
[[nodiscard]] constexpr auto div(int numer, int denom) noexcept
        -> div_t
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

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
[[nodiscard]] constexpr auto euclidean_div(int numer, int denom) noexcept
        -> div_t
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

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
[[nodiscard]] constexpr auto floored_div(int numer, int denom) noexcept
        -> div_t
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

}       // namespace xstd

template<>
struct std::formatter<xstd::div_t> : std::formatter<std::tuple<int const&, int const&>>
{
        auto format(xstd::div_t const& d, auto& ctx) const
        {
                return std::formatter<std::tuple<int const&, int const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

namespace xstd {

inline auto& operator<<(std::ostream& ostr, div_t const& d)
{
        ostr << std::format("{}", d);
        return ostr;
}

}       // namespace xstd
