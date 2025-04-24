//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstddef.hpp>             // _uz, size_t
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL
#include <type_traits>                  // is_same_v

BOOST_AUTO_TEST_SUITE(CStdDef)

BOOST_AUTO_TEST_CASE(UserDefinedLiteralSizeT)
{
        using namespace xstd::support_literals;
        static_assert(std::is_same_v<decltype(0_uz), std::size_t>);
        static_assert(0_uz == static_cast<std::size_t>(0));

        // repeat previous static_assert at runtime for code coverage purposes
        BOOST_CHECK_EQUAL(0_uz, static_cast<std::size_t>(0));
}

BOOST_AUTO_TEST_SUITE_END()
