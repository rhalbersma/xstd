//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/core/concepts.hpp>   // specialization_of
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Core)
BOOST_AUTO_TEST_SUITE(Concepts)

template<class>
struct box
{};

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        static_assert(xstd::specialization_of<box<int>, box>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
