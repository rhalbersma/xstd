//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/exhaustive.hpp>
#include <int_set/primitive.hpp>
#include <xstd/bitset.hpp>
#include <xstd/int_set.hpp>
#include <boost/mpl/vector.hpp>                         // vector
#include <boost/test/test_case_template.hpp>            // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>                     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Cubic)

using namespace xstd;

using SetTypes = boost::mpl::vector
<
        std::bitset<32>,
            int_set<32>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(ExhaustivelyTestAssociativityAndDistributivity, T, SetTypes)
{
        all_singlet_triples<T>(test::op_bitand{});
        all_singlet_triples<T>(test::op_bitor{});
        all_singlet_triples<T>(test::op_xor{});
}

BOOST_AUTO_TEST_SUITE_END()
