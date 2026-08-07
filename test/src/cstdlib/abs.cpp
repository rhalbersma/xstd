//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/abs.hpp>            // abs
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <limits>                          // numeric_limits

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE_TEMPLATE(Abs, T, xstd::test::exact_width_signed_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-2}), T{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{0}), T{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+2}), T{2});

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(static_cast<T>(limits::min() + 1)), limits::max());
}

BOOST_AUTO_TEST_SUITE_END()
