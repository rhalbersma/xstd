//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/exhaustive.hpp>
#include <int_set/primitive.hpp>
#include <xstd/bitset.hpp>                      // bitset
#include <xstd/int_set.hpp>                     // int_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <sstream>                              // stringstream

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using SetTypes = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset<  1>
,       std::bitset<  2>
,       std::bitset< 32>
,       std::bitset< 33>
,       std::bitset< 34>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set<  2, uint32_t>
,       int_set< 32, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 35, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        all_doubleton_arrays<T>([](auto const& a2) {
                test::constructor<T>{}(a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                test::constructor<T>{}(ilist2);
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                test::assign{}(~T{}, a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                test::assign{}(~T{}, ilist2);
        });

        all_doubleton_sets<T>(test::const_reference{});
        all_doubleton_sets<T>(test::const_iterator{});

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1) {
                        test::any_of{}(i1, [&](auto const elem) {
                                return elem == pos;
                        });
                });
        });

        all_doubleton_sets<T>(test::for_each{});
        all_doubleton_sets<T>(test::reverse_for_each{});

        all_singleton_set_pairs<T>(test::op_bitand_assign{});
        all_singleton_set_pairs<T>(test::op_bitor_assign{});
        all_singleton_set_pairs<T>(test::op_xor_assign{});
        all_singleton_set_pairs<T>(test::op_minus_assign{});

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1){
                        test::op_shift_left_assign{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1){
                        test::op_shift_right_assign{}(i1, pos);
                });
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                test::insert{}(T{}, a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                test::insert{}(T{}, ilist2);
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                test::erase{}(~T{}, a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                test::erase{}(T{}, ilist2);
        });

        all_singleton_set_pairs<T>(test::swap_{});

        all_singleton_set_pairs<T>(test::op_compl{});

        all_singleton_set_pairs<T>(test::op_equal_to{});
        all_singleton_set_pairs<T>(test::op_not_equal_to{});

        all_singleton_set_pairs<T>(test::op_less{});
        all_singleton_set_pairs<T>(test::op_greater{});
        all_singleton_set_pairs<T>(test::op_greater_equal{});
        all_singleton_set_pairs<T>(test::op_less_equal{});

        all_singleton_set_pairs<T>(test::intersects_{});
        all_singleton_set_pairs<T>(test::disjoint_{});

        all_singleton_set_pairs<T>(test::is_subset_of_{});
        all_singleton_set_pairs<T>(test::is_superset_of_{});
        all_singleton_set_pairs<T>(test::is_proper_subset_of_{});
        all_singleton_set_pairs<T>(test::is_proper_superset_of_{});

        all_values<T>([](auto const pos){
                all_singleton_sets<T>([&](auto const& i1){
                        test::op_shift_left{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos){
                all_singleton_sets<T>([&](auto const& i1){
                        test::op_shift_right{}(i1, pos);
                });
        });

        all_singleton_set_pairs<T>(test::op_bitand{});
        all_singleton_set_pairs<T>(test::op_bitor{});
        all_singleton_set_pairs<T>(test::op_xor{});
        all_singleton_set_pairs<T>(test::op_minus{});

        all_doubleton_sets<T>([](auto const& i2) {
                std::stringstream sstr;
                test::op_ostream{}(sstr, i2);
        });
}

BOOST_AUTO_TEST_SUITE_END()
