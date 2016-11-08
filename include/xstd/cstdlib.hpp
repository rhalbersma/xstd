#pragma once
#include <cassert>      // assert
#include <cstdlib>      // div_t
#include <tuple>        // tie

namespace xstd {

// a constexpr version of std::abs(int)
constexpr auto abs(int n) noexcept
{
        return n < 0 ? -n : n;
}

// http://stackoverflow.com/a/4609795/819272
constexpr auto signum(int n) noexcept
{
        return static_cast<int>(0 < n) - static_cast<int>(n < 0);
}

// The following is discussed in the C++ Standard [expr.mul]/4 and
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// a constexpr version of std::div(int, int)
constexpr auto truncated_div(int D, int d) // Throws: Nothing.
{
        assert(d != 0);
        auto const qT = D / d;
        auto const rT = D % d;
        assert(D == d * qT + rT);
        assert(abs(rT) < abs(d));
        assert(signum(rT) == signum(D) || rT == 0);
        return std::div_t{qT, rT};
}

constexpr auto floored_div(int D, int d) // Throws: Nothing.
{
        assert(d != 0);
        auto const divT = truncated_div(D, d);
        auto const I = signum(divT.rem) == -signum(d) ? 1 : 0;
        auto const qF = divT.quot - I;
        auto const rF = divT.rem + I * d;
        assert(D == d * qF + rF);
        assert(abs(rF) < abs(d));
        assert(signum(rF) == signum(d));
        return std::div_t{qF, rF};
}

constexpr auto euclidean_div(int D, int d) // Throws: Nothing.
{
        assert(d != 0);
        auto const divT = truncated_div(D, d);
        auto const I = divT.rem >= 0 ? 0 : (d > 0 ? 1 : -1);
        auto const qE = divT.quot - I;
        auto const rE = divT.rem + I * d;
        assert(D == d * qE + rE);
        assert(abs(rE) < abs(d));
        assert(signum(rE) != -1);
        return std::div_t{qE, rE};
}

constexpr auto tied(std::div_t const& d) noexcept
{
        return std::tie(d.quot, d.rem);
}

constexpr auto operator==(std::div_t const& lhs, std::div_t const& rhs) noexcept
{
        return tied(lhs) == tied(rhs);
}

constexpr auto operator!=(std::div_t const& lhs, std::div_t const& rhs) noexcept
{
        return !(lhs == rhs);
}

}       // namespace xstd
