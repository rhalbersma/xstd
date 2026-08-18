//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/floored_div.hpp>             // floored_div
#include <xstd/test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <xstd/test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>                 // Boost.Test

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE_TEMPLATE(FlooredDiv, T, xstd::test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+8}, T{+3})), (xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+8}, T{-3})), (xstd::div_t<T>{-3, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-8}, T{+3})), (xstd::div_t<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-8}, T{-3})), (xstd::div_t<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+1}, T{+2})), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+1}, T{-2})), (xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-1}, T{+2})), (xstd::div_t<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-1}, T{-2})), (xstd::div_t<T>{0, -1}));
}

// Unsigned, the three conventions coincide; checked against xstd::div, not against literals.
BOOST_AUTO_TEST_CASE_TEMPLATE(FlooredDivUnsigned, T, xstd::test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{8}, T{3})), (xstd::div_t<T>{2, 2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{1}, T{2})), (xstd::div_t<T>{0, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{0}, T{3})), (xstd::div_t<T>{0, 0}));
}

BOOST_AUTO_TEST_SUITE_END()
