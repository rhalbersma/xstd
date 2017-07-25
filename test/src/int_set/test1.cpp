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
        all_singleton_arrays<T>([](auto const& a1) {
                test::constructor<T>{}(a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                test::constructor<T>{}(ilist1);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                test::assign{}(~T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                test::assign{}(~T{}, ilist1);
        });

        all_cardinality_sets<T>(test::const_reference{});
        all_singleton_sets<T>(test::const_reference{});

        all_cardinality_sets<T>(test::const_iterator{});
        all_singleton_sets<T>(test::const_iterator{});

        all_cardinality_sets<T>(test::front{});
        all_singleton_sets<T>(test::front{});

        all_cardinality_sets<T>(test::back{});
        all_singleton_sets<T>(test::back{});

        all_cardinality_sets<T>(test::for_each_{});
        all_singleton_sets<T>(test::for_each_{});

        all_cardinality_sets<T>(test::reverse_for_each_{});
        all_singleton_sets<T>(test::reverse_for_each_{});

        all_values<T>([](auto const pos) {
                test::set{}(T{}, pos);
                test::set{}(T{}, pos, true);
                test::set{}(T{}, pos, false);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                test::insert{}(T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                test::insert{}(T{}, ilist1);
        });

        all_cardinality_sets<T>(test::reset{});
        all_values<T>([](auto const pos) {
                test::reset{}(~T{}, pos);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                test::erase{}(~T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                test::erase{}(T{}, ilist1);
        });

        all_cardinality_sets<T>(test::op_compl{});
        all_singleton_sets<T>(test::op_compl{});

        all_values<T>([](auto const pos) {
                test::flip{}(T{}, pos);
        });

        all_cardinality_sets<T>(test::count{});
        all_cardinality_sets<T>(test::size{});

        all_cardinality_sets<T>(test::op_less{});
        all_cardinality_sets<T>(test::op_greater{});
        all_cardinality_sets<T>(test::op_greater_equal{});
        all_cardinality_sets<T>(test::op_less_equal{});

        all_cardinality_sets<T>(test::is_subset_of_{});
        all_cardinality_sets<T>(test::is_superset_of_{});
        all_cardinality_sets<T>(test::is_proper_subset_of_{});
        all_cardinality_sets<T>(test::is_proper_superset_of_{});

        all_values<T>([](auto const pos) {
                test::test_{}(T{}, pos);
        });

        all_cardinality_sets<T>(test::any{});
        all_cardinality_sets<T>(test::none{});

        all_cardinality_sets<T>(test::op_bitand{});
        all_singleton_sets<T>(test::op_bitand{});

        all_cardinality_sets<T>(test::op_bitor{});
        all_singleton_sets<T>(test::op_bitor{});

        all_cardinality_sets<T>(test::op_xor{});
        all_singleton_sets<T>(test::op_xor{});

        all_cardinality_sets<T>(test::op_minus{});
        all_singleton_sets<T>(test::op_minus{});

        all_cardinality_sets<T>(test::intersects_{});
        all_singleton_sets<T>(test::intersects_{});

        all_cardinality_sets<T>(test::disjoint_{});
        all_singleton_sets<T>(test::disjoint_{});
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
        all_cardinality_sets<T>(test::set{});
        all_cardinality_sets<T>(test::flip{});
        all_cardinality_sets<T>(test::all{});
}

BOOST_AUTO_TEST_SUITE_END()
