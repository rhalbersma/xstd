//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/limits.hpp>               // numeric_limits
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <limits>                        // numeric_limits

BOOST_AUTO_TEST_SUITE(Limits)

BOOST_AUTO_TEST_CASE(DelegatesToTheStandardDomain)
{
        static_assert(xstd::numeric_limits<int>::is_specialized == std::numeric_limits<int>::is_specialized);
        static_assert(xstd::numeric_limits<int const>::digits == std::numeric_limits<int>::digits);
        static_assert(xstd::numeric_limits<unsigned volatile>::is_signed == std::numeric_limits<unsigned>::is_signed);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::numeric_limits<int>::min(), std::numeric_limits<int>::min());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::numeric_limits<int>::max(), std::numeric_limits<int>::max());
}

BOOST_AUTO_TEST_SUITE_END()
