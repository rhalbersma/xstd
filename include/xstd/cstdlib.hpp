#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <cstddef>      // size_t
#include <iosfwd>       // basic_ostream
#include <tuple>        // tie
#include <type_traits>  // enable_if_t, is_integral_v, is_signed_v

namespace xstd {

template<class T, std::enable_if_t<
        std::is_signed_v<T> && std::is_integral_v<T>
>...>
constexpr auto abs(T const n) noexcept
{
        return n < 0 ? -n : n;
}

template<class T, std::enable_if_t<
        std::is_signed_v<T> && std::is_integral_v<T>
>...>
constexpr auto sign(T const n) noexcept
{
        return static_cast<T>(0 < n) - static_cast<T>(n < 0);
}

struct div_t { int quot, rem; };

constexpr auto operator==(div_t const& lhs, div_t const& rhs) noexcept
{
        constexpr auto tied = [](auto const& d) {
                return std::tie(d.quot, d.rem);
        };
        return tied(lhs) == tied(rhs);
}

constexpr auto operator!=(div_t const& lhs, div_t const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<class CharT, class Traits>
auto& operator<<(std::basic_ostream<CharT, Traits>& ostr, div_t const& d)
{
        return ostr << '[' << d.quot << ',' << d.rem << ']';
}

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// a constexpr version of std::div(int, int)
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
constexpr auto div(int D, int d) // Throws: Nothing.
        -> div_t
{
        assert(d != 0);
        auto const qT = D / d;
        auto const rT = D % d;
        assert(D == d * qT + rT);
        assert(abs(rT) < abs(d));
        assert(sign(rT) == sign(D)|| rT == 0);
        return { qT, rT };
}

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
constexpr auto euclidean_div(int D, int d) // Throws: Nothing.
        -> div_t
{
        assert(d != 0);
        auto const divT = div(D, d);
        auto const I = divT.rem >= 0 ? 0 : (d > 0 ? 1 : -1);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + I * d;
        assert(D == d * qE + rE);
        assert(abs(rE) < abs(d));
        assert(sign(rE) >= 0);
        return { qE, rE };
}

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
constexpr auto floored_div(int D, int d) // Throws: Nothing.
        -> div_t
{
        assert(d != 0);
        auto const divT = div(D, d);
        auto const I = sign(divT.rem) == -sign(d) ? 1 : 0;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + I * d;
        assert(D == d * qF + rF);
        assert(abs(rF) < abs(d));
        assert(sign(rF) == sign(d));
        return { qF, rF };
}

}       // namespace xstd
