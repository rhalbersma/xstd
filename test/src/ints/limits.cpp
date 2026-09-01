//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/limits.hpp>     // numeric_limits
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                 // derived_from
#include <limits>                   // numeric_limits

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Limits)

// One trait behind this door, so no invariant spans two; that it arrives is what the umbrella answers for.
BOOST_AUTO_TEST_CASE(TheTraitArrivesThroughTheUmbrella)
{
        static_assert(std::derived_from<xstd::numeric_limits<int>, std::numeric_limits<int>>);
        static_assert(xstd::numeric_limits<int>::is_specialized);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
