//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/primitive.hpp>
#include <xstd/bitset.hpp>                      // bitset
#include <xstd/int_set.hpp>                     // int_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Constant)

using namespace xstd;

using SetTypes = boost::mpl::vector
<
        std::bitset<  0>,
        std::bitset<  1>,
        std::bitset< 33>,
        std::bitset< 65>,
        std::bitset<128>,
            int_set<  0>,
            int_set<  1>,
            int_set< 32>,
            int_set< 64>,
            int_set<128>,
            int_set<256>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        test::constructor<T>{}();
        test::capacity<T>{}();
        test::test_{}(T{});
}

BOOST_AUTO_TEST_SUITE_END()
