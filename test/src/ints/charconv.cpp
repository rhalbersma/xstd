//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/charconv.hpp>   // to_chars, to_chars_max_size
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <array>                    // array

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CharConv)

// The front door opens on every room behind it: reached through the umbrella alone.
BOOST_AUTO_TEST_CASE(ReExportsTheWholeDirectory)
{
        static_assert(xstd::to_chars_max_size<int> > 0);
        auto buffer = std::array<char, 32>{};
        static_assert(requires { xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), 0); });
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
