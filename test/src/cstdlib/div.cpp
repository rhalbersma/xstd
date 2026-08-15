//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/div.hpp>            // div
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types, exact_width_unsigned_types
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <limits>                          // numeric_limits

BOOST_AUTO_TEST_SUITE(CStdLib)

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatedDiv, T, xstd::test::exact_width_signed_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+8}, T{+3})), (xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+8}, T{-3})), (xstd::div_t<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-8}, T{+3})), (xstd::div_t<T>{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-8}, T{-3})), (xstd::div_t<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+1}, T{+2})), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+1}, T{-2})), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-1}, T{+2})), (xstd::div_t<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-1}, T{-2})), (xstd::div_t<T>{0, -1}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatedDivUnsigned, T, xstd::test::exact_width_unsigned_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{8}, T{3})), (xstd::div_t<T>{2, 2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{1}, T{2})), (xstd::div_t<T>{0, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{0}, T{3})), (xstd::div_t<T>{0, 0}));

        // numer == min() and denom == static_cast<T>(-1) at the same time,
        // which over an unsigned type is 0 and max() rather than a quotient
        // that does not exist. The signed precondition is not asked here.
        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(limits::min(), limits::max())), (xstd::div_t<T>{0, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(limits::max(), limits::max())), (xstd::div_t<T>{1, 0}));
}

BOOST_AUTO_TEST_SUITE_END()
