//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/type_traits/no_unique_address.hpp> // XSTD_NO_UNIQUE_ADDRESS
#include <boost/test/unit_test.hpp>                    // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(NoUniqueAddress)

struct empty
{};

struct storage
{
        [[XSTD_NO_UNIQUE_ADDRESS]] empty value;
        int payload;
};

BOOST_AUTO_TEST_CASE(EmptyMemberNeedsNoAdditionalStorage)
{
        static_assert(sizeof(storage) == sizeof(int));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
