//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/test/constexpr.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(SignedIntegralLike)
{
        XSTD_CONSTEXPR_CHECK(xstd::signed_integral_like<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::signed_integral_like<unsigned>);
        XSTD_CONSTEXPR_CHECK(not xstd::signed_integral_like<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::signed_integral_like<int const>);
}

BOOST_AUTO_TEST_SUITE_END()
