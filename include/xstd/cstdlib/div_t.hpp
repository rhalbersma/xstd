//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_DIV_T_HPP
#define XSTD_CSTDLIB_DIV_T_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like

namespace xstd {

template<signed_integral_like S>
struct div_t
{
        S quot, rem;
        // Neither constexpr nor an exception specification, deliberately.
        // A function explicitly defaulted on its first declaration is
        // implicitly constexpr if the implicit declaration would be, and gets
        // an exception specification computed from what it does - here the two
        // member comparisons and nothing else, since [iterator.concept.winc]
        // /7.6 pins S's operator== to bool and leaves no conversion to throw.
        // [[nodiscard]] is the only one of the three with no implicit form, so
        // it is the only one written.
        //
        // Restating either of the others would agree with the computed answer
        // today, which is the whole of what it buys, and they fail differently
        // on the day it stops agreeing. A constexpr that cannot hold is
        // refused where a constant expression needs it. A noexcept that cannot
        // hold is not refused at all: since P1286R2 (C++20) a defaulted
        // function whose declared specification disagrees with the implicit
        // one is no longer deleted - the declared one simply wins - so it buys
        // a std::terminate rather than a diagnostic, and one member of a type
        // other than S would be enough to earn it.
        //
        // Which makes this the one place in the library that has what the six
        // functions in <xstd/cstdlib.hpp> cannot: `= default` is already
        // noexcept(auto). They carry xstd::nothrow_arithmetic because a
        // written body gets no such deduction. See doc/design.md.
        [[nodiscard]] friend auto operator==(div_t const&, div_t const&) -> bool = default;
};

// Explicit to keep -Wctad-maybe-unsupported quiet.
template<signed_integral_like S>
div_t(S, S) -> div_t<S>;

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_T_HPP
