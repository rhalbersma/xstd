//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib.hpp>                    // complete arithmetic surface
#include <xstd/test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <xstd/test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp>          // exact-width integer type lists
#include <xstd/ints/limits.hpp>                     // numeric_limits
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE

BOOST_AUTO_TEST_SUITE(CStdLib)

// Without negative values, truncated, floored and Euclidean division are the same operation.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedDivisionConventionsAgree, T, xstd::test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>, (xstd::div(T{8}, T{3})), (xstd::div_euclid(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>, (xstd::div(T{8}, T{3})), (xstd::div_floor(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>, (xstd::div(T{1}, T{2})), (xstd::div_euclid(T{1}, T{2})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>, (xstd::div(T{1}, T{2})), (xstd::div_floor(T{1}, T{2})));

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>,
                                      (xstd::div(limits::max(), T{3})), (xstd::div_euclid(limits::max(), T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>,
                                      (xstd::div(limits::max(), T{3})), (xstd::div_floor(limits::max(), T{3})));
}

// An exact division has no convention-dependent remainder, even for signed operands.
BOOST_AUTO_TEST_CASE_TEMPLATE(ExactDivisionConventionsAgree, T, xstd::test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>, (xstd::div(T{-6}, T{3})), (xstd::div_euclid(T{-6}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(xstd::test::has_constexpr_division<T>, (xstd::div(T{-6}, T{3})), (xstd::div_floor(T{-6}, T{3})));
}

BOOST_AUTO_TEST_SUITE_END()
