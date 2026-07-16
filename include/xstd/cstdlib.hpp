#pragma once

//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <format>       // format, formatter
#include <ios>          // ios_base
#include <iosfwd>       // basic_istream, basic_ostream
#include <limits>       // numeric_limits
#include <tuple>        // tie, tuple
#include <type_traits>  // is_arithmetic_v

namespace xstd {

template<class T>
[[nodiscard]] constexpr auto abs(T const& x) noexcept
        requires std::is_arithmetic_v<T>
{
        return x < 0 ? -x : x;
}

template<class T>
[[nodiscard]] constexpr auto sign(T const& x) noexcept
        requires std::is_arithmetic_v<T>
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

template<class CharT>
struct std::formatter<xstd::div_t, CharT>
        : std::formatter<std::tuple<int const&, int const&>, CharT>
{
        template<class FormatContext>
        auto format(xstd::div_t const& d, FormatContext& ctx) const
        {
                return std::formatter<std::tuple<int const&, int const&>, CharT>::format(std::tie(d.quot, d.rem), ctx);
        }
};

namespace xstd {

// The stream inserters delegate to std::format (hence they are defined below
// the formatter specialization), so that streaming and formatting a div_t
// produce identical text. The .c_str() detour keeps them usable with any
// Traits, which the basic_string inserter would reject.

template<class Traits>
auto& operator<<(std::basic_ostream<char, Traits>& ostr, div_t const& d)
{
        return ostr << std::format("{}", d).c_str();
}

template<class Traits>
auto& operator<<(std::basic_ostream<wchar_t, Traits>& ostr, div_t const& d)
{
        return ostr << std::format(L"{}", d).c_str();
}

// Extracts a div_t from text of the form "(quot, rem)", as produced by
// operator<< and std::format. Malformed input is a runtime condition, not a
// contract violation, so - following the conventions of the standard
// library's extractors - it sets failbit and leaves d unmodified.
template<class CharT, class Traits>
auto& operator>>(std::basic_istream<CharT, Traits>& istr, div_t& d)
{
        auto const expect = [&istr](char token) {
                auto c = CharT();
                if (istr >> c && !Traits::eq(c, istr.widen(token))) {
                        istr.setstate(std::ios_base::failbit);
                }
        };
        auto parsed = div_t{};
        expect('(');
        istr >> parsed.quot;
        expect(',');
        istr >> parsed.rem;
        expect(')');
        if (istr) {
                d = parsed;
        }
        return istr;
}

}       // namespace xstd
