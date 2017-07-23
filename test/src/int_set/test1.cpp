//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/exhaustive.hpp>
#include <int_set/primitive.hpp>
#include <xstd/bitset.hpp>
#include <xstd/int_set.hpp>
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using SetTypes = boost::mpl::vector
<
        std::bitset< 0>,
        std::bitset< 1>,
        std::bitset<33>,
        std::bitset<65>,
            int_set< 0, uint32_t>,
            int_set< 1, uint32_t>,
            int_set<33, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        all_singlets<T>(test::const_reference{});
        all_counts<T>(test::const_reference{});

        all_counts<T>(test::const_iterator{});
        all_singlets<T>(test::const_iterator{});

        all_counts<T>(test::for_each_{});
        all_singlets<T>(test::for_each_{});

        all_counts<T>(test::reverse_for_each_{});
        all_singlets<T>(test::reverse_for_each_{});

        all_values<T>([](auto const pos) {
                test::set{}(T{}, pos);
                test::set{}(T{}, pos, true);
                test::set{}(T{}, pos, false);
        });

        all_counts<T>(test::reset{});
        all_values<T>([](auto const pos) {
                test::reset{}(~T{}, pos);
        });

        all_counts<T>(test::op_compl{});
        all_singlets<T>(test::op_compl{});

        all_values<T>([](auto const pos) {
                test::flip{}(T{}, pos);
        });

        all_counts<T>(test::count{});
        all_counts<T>(test::size{});

        all_values<T>([](auto const pos) {
                test::test_{}(T{}, pos);
        });

        all_counts<T>(test::any{});
        all_counts<T>(test::none{});

        all_counts<T>(test::op_bitand{});
        all_singlets<T>(test::op_bitand{});

        all_counts<T>(test::op_bitor{});
        all_singlets<T>(test::op_bitor{});

        all_counts<T>(test::op_xor{});
        all_singlets<T>(test::op_xor{});

        all_counts<T>(test::op_minus{});
        all_singlets<T>(test::op_minus{});
}

using SetTypes2 = boost::mpl::vector
<
        std::bitset<  0>,
        std::bitset<  1>,
        std::bitset< 32>,
        std::bitset< 33>,
        std::bitset< 64>,
        std::bitset< 65>,
        std::bitset<128>,
            int_set<  0, uint32_t>,
            int_set<  1, uint32_t>,
            int_set< 32, uint32_t>,
            int_set< 33, uint32_t>,
            int_set< 64, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive2, T, SetTypes2)
{
        all_counts<T>(test::set{});
        all_counts<T>(test::flip{});
        all_counts<T>(test::all{});
}

BOOST_AUTO_TEST_SUITE_END()
