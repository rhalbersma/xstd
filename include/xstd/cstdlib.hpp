#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <cstddef>      // size_t
#include <iosfwd>       // basic_istream, basic_ostream
#include <tuple>        // tie
#include <type_traits>  // is_arithmetic_v

namespace xstd {

template<class T>
        requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr auto abs(T const& x) noexcept
{
        return x < 0 ? -x : x;
}

template<class T>
        requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr auto sign(T const& x) noexcept
        -> int
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

struct div_t 
{ 
        int quot, rem; 
        [[nodiscard]] bool operator==(div_t const&) const = default;
};

template<class CharT, class Traits>
auto& operator<<(std::basic_ostream<CharT, Traits>& ostr, div_t const& d)
{
        return ostr << ostr.widen('[') << d.quot << ostr.widen(',') << d.rem << ostr.widen(']');
}

template<class CharT, class Traits>
auto& operator>>(std::basic_istream<CharT, Traits>& istr, div_t& d)
{
        CharT c;
        istr >> c; assert(c == istr.widen('['));
        istr >> d.quot;
        istr >> c; assert(c == istr.widen(','));
        istr >> d.rem;
        istr >> c; assert(c == istr.widen(']'));
        return istr;
}

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// a constexpr version of std::div(int, int)
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
[[nodiscard]] constexpr auto div(int D, int d) // Throws: Nothing.
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
[[nodiscard]] constexpr auto euclidean_div(int D, int d) // Throws: Nothing.
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
[[nodiscard]] constexpr auto floored_div(int D, int d) // Throws: Nothing.
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
