//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/floored_div.hpp>    // floored_div
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types
#include <boost/test/unit_test.hpp>        // Boost.Test

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE_TEMPLATE(FlooredDiv, T, xstd::test::exact_width_signed_types)
{
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+8}, T{+3}) == xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+8}, T{-3}) == xstd::div_t<T>{-3, -1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-8}, T{+3}) == xstd::div_t<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-8}, T{-3}) == xstd::div_t<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+1}, T{+2}) == xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+1}, T{-2}) == xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-1}, T{+2}) == xstd::div_t<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-1}, T{-2}) == xstd::div_t<T>{0, -1}));
}

BOOST_AUTO_TEST_SUITE_END()
