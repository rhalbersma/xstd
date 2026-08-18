//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>                         // complete arithmetic surface
#include <xstd/test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <xstd/test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp>          // exact-width integer type lists
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE
#include <limits>                                   // numeric_limits

BOOST_AUTO_TEST_SUITE(CStdLib)

// Without negative values, truncated, floored and Euclidean division are the same operation.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedDivisionConventionsAgree, T, xstd::test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{8}, T{3})), (xstd::euclidean_div(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{8}, T{3})), (xstd::floored_div(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{1}, T{2})), (xstd::euclidean_div(T{1}, T{2})));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{1}, T{2})), (xstd::floored_div(T{1}, T{2})));

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(
                (xstd::div(limits::max(), T{3})), (xstd::euclidean_div(limits::max(), T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL(
                (xstd::div(limits::max(), T{3})), (xstd::floored_div(limits::max(), T{3})));
}

// An exact division has no convention-dependent remainder, even for signed operands.
BOOST_AUTO_TEST_CASE_TEMPLATE(ExactDivisionConventionsAgree, T, xstd::test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-6}, T{3})), (xstd::euclidean_div(T{-6}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-6}, T{3})), (xstd::floored_div(T{-6}, T{3})));
}

BOOST_AUTO_TEST_SUITE_END()
