//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/core/utility.hpp>    // to_underlying
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <type_traits>              // integral_constant

BOOST_AUTO_TEST_SUITE(Core)
BOOST_AUTO_TEST_SUITE(Utility)

enum class shade : int { one = 1 };

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        static_assert(xstd::to_underlying(std::integral_constant<shade, shade::one>{}) == 1);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
