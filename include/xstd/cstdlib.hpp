#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <cstddef>      // size_t
#include <tuple>        // tie

namespace xstd {

constexpr bool is_power_of_2(std::size_t x) noexcept
{
        return (x - 1) < (x & -x);
}

constexpr std::size_t align_on(std::size_t address, std::size_t alignment) noexcept
{
        assert(is_power_of_2(alignment));
        return (address + alignment - 1) & ~(alignment - 1);
}

// a constexpr version of std::abs(int)
constexpr int abs(int n)
{
        return n < 0 ? -n : n;
}

// http://stackoverflow.com/a/4609795/819272
constexpr int signum(int n) noexcept
{
        return static_cast<int>(0 < n) - static_cast<int>(n < 0);
}

struct div_t { int quot, rem; };

constexpr bool operator==(div_t const& lhs, div_t const& rhs) noexcept
{
        return
                std::tie(lhs.quot, lhs.rem) ==
                std::tie(rhs.quot, rhs.rem)
        ;
}

constexpr bool operator!=(div_t const& lhs, div_t const& rhs) noexcept
{
        return !(lhs == rhs);
}

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// a constexpr version of std::div(int, int)
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
constexpr div_t truncated_div(int D, int d) // Throws: Nothing.
{
        assert(d != 0);
        auto const qT = D / d;
        auto const rT = D % d;
        assert(D == d * qT + rT);
        assert(abs(rT) < abs(d));
        assert(signum(rT) == signum(D)|| rT == 0);
        return { qT, rT };
}

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
constexpr div_t floored_div(int D, int d) // Throws: Nothing.
{
        assert(d != 0);
        auto const divT = truncated_div(D, d);
        auto const I = signum(divT.rem) == -signum(d) ? 1 : 0;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + I * d;
        assert(D == d * qF + rF);
        assert(abs(rF) < abs(d));
        assert(signum(rF) == signum(d));
        return { qF, rF };
}

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
constexpr div_t euclidean_div(int D, int d) // Throws: Nothing.
{
        assert(d != 0);
        auto const divT = truncated_div(D, d);
        auto const I = divT.rem >= 0 ? 0 : (d > 0 ? 1 : -1);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + I * d;
        assert(D == d * qE + rE);
        assert(abs(rE) < abs(d));
        assert(signum(rE) >= 0);
        return { qE, rE };
}

}       // namespace xstd
