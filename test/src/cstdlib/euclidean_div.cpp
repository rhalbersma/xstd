//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/euclidean_div.hpp>  // euclidean_div
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types, exact_width_unsigned_types
#include <xstd/cstdlib/div.hpp>            // div
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <limits>                          // numeric_limits

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE_TEMPLATE(EuclideanDiv, T, xstd::test::exact_width_signed_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+8}, T{+3})), (xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+8}, T{-3})), (xstd::div_t<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-8}, T{+3})), (xstd::div_t<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-8}, T{-3})), (xstd::div_t<T>{+3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+1}, T{+2})), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+1}, T{-2})), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-1}, T{+2})), (xstd::div_t<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-1}, T{-2})), (xstd::div_t<T>{+1, +1}));
}

// Unsigned, the three conventions coincide; checked against xstd::div, not against literals.
BOOST_AUTO_TEST_CASE_TEMPLATE(EuclideanDivUnsigned, T, xstd::test::exact_width_unsigned_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{8}, T{3})), (xstd::div_t<T>{2, 2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{1}, T{2})), (xstd::div_t<T>{0, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{0}, T{3})), (xstd::div_t<T>{0, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{8}, T{3})), (xstd::div(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{1}, T{2})), (xstd::div(T{1}, T{2})));

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(limits::max(), T{3})), (xstd::div(limits::max(), T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(limits::min(), limits::max())), (xstd::div(limits::min(), limits::max())));
}

BOOST_AUTO_TEST_SUITE_END()
