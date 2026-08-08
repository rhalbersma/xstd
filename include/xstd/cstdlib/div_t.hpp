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
        // No exception specification, deliberately. A defaulted comparison
        // already has one computed from what it does, and what it does is the
        // two member comparisons and nothing else: [iterator.concept.winc]/7.6
        // pins S's operator== to bool, so the contextual conversion is an
        // identity with nothing to throw. Writing
        // noexcept(noexcept(std::declval<S const&>() == std::declval<S const&>()))
        // would restate that computation, in the one place where restating it
        // wrongly is not diagnosed: since P1286R2 (C++20) a defaulted function
        // whose declared specification disagrees with the implicit one is no
        // longer deleted - the declared one simply wins - so an over-broad
        // noexcept here buys a std::terminate rather than a compile error, and
        // one member of some other type would be enough to earn it.
        //
        // Which makes this the one place in the library that has what the six
        // functions in <xstd/cstdlib.hpp> cannot: `= default` with no specifier
        // *is* noexcept(auto). They carry xstd::nothrow_arithmetic because a
        // written body gets no such deduction. See doc/design.md.
        [[nodiscard]] friend constexpr auto operator==(div_t const&, div_t const&) -> bool = default;
};

// Explicit to keep -Wctad-maybe-unsupported quiet.
template<signed_integral_like S>
div_t(S, S) -> div_t<S>;

} // namespace xstd

#endif // XSTD_CSTDLIB_DIV_T_HPP
