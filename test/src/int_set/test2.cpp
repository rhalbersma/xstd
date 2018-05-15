//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>                               // bitset
#include <xstd/int_set.hpp>                     // int_set
#include <int_set/legacy/bitset.hpp>            // bitset
#include <int_set/legacy/int_set.hpp>           // int_set
#include <int_set/exhaustive.hpp>               // all_values, all_singleton_sets, all_singleton_set_pairs,
                                                // all_doubleton_arrays, all_doubleton_ilists, all_doubleton_sets,
#include <int_set/primitive.hpp>                // constructor, mem_assign, const_reference, const_iterator, mem_all_of, mem_any_of, me_none_of,
                                                // mem_for_each, mem_reverse_for_each, op_bitand_assign, op_bitor_assign, op_xor_assign, op_minus_assign,
                                                // op_shift_left_assign, op_shift_right_assign, mem_insert, mem_erase, fn_swap, op_compl,
                                                // op_equal_to, op_not_equal_to, op_less, op_greater, op_greater_equal, op_less_equal,
                                                // fn_is_subset_of, fn_is_superset_of, fn_is_proper_subset_of, fn_is_proper_superset_of,
                                                // fn_intersect, fn_disjoint, op_shift_left, op_shift_right, op_bitand, op_bitor, op_xor, op_minus
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using SetTypes = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset< 32>
,       std::bitset< 64>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 65, uint32_t>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        all_doubleton_arrays<T>([](auto const& a2) {
                constructor<T>{}(a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                constructor<T>{}(ilist2);
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                mem_assign{}(~T{}, a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                mem_assign{}(~T{}, ilist2);
        });

        all_doubleton_sets<T>(const_reference{});
        all_doubleton_sets<T>(const_iterator{});

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1) {
                        mem_any_of{}(i1, [&](auto const elem) {
                                return elem == pos;
                        });
                });
        });

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1) {
                        mem_none_of{}(i1, [&](auto const elem) {
                                return elem == pos;
                        });
                });
        });

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1) {
                        mem_all_of{}(i1, [&](auto const elem) {
                                return elem == pos;
                        });
                });
        });

        all_doubleton_sets<T>(mem_for_each{});
        all_doubleton_sets<T>(mem_reverse_for_each{});

        all_singleton_set_pairs<T>(op_bitand_assign{});
        all_singleton_set_pairs<T>(op_bitor_assign{});
        all_singleton_set_pairs<T>(op_xor_assign{});
        all_singleton_set_pairs<T>(op_minus_assign{});

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1){
                        op_shift_left_assign{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos) {
                all_singleton_sets<T>([&](auto const& i1){
                        op_shift_right_assign{}(i1, pos);
                });
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                mem_insert{}(T{}, a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                mem_insert{}(T{}, ilist2);
        });

        all_doubleton_arrays<T>([](auto const& a2) {
                mem_erase{}(~T{}, a2.begin(), a2.end());
        });
        all_doubleton_ilists<T>([](auto ilist2) {
                mem_erase{}(T{}, ilist2);
        });

        all_singleton_set_pairs<T>(fn_swap{});

        all_singleton_set_pairs<T>(op_compl{});

        all_singleton_set_pairs<T>(op_equal_to{});
        all_singleton_set_pairs<T>(op_not_equal_to{});

        all_singleton_set_pairs<T>(op_less{});
        all_singleton_set_pairs<T>(op_greater{});
        all_singleton_set_pairs<T>(op_greater_equal{});
        all_singleton_set_pairs<T>(op_less_equal{});

        all_singleton_set_pairs<T>(fn_is_subset_of{});
        all_singleton_set_pairs<T>(fn_is_superset_of{});
        all_singleton_set_pairs<T>(fn_is_proper_subset_of{});
        all_singleton_set_pairs<T>(fn_is_proper_superset_of{});

        all_singleton_set_pairs<T>(fn_intersects{});
        all_singleton_set_pairs<T>(fn_disjoint{});

        all_values<T>([](auto const pos){
                all_singleton_sets<T>([&](auto const& i1){
                        op_shift_left{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos){
                all_singleton_sets<T>([&](auto const& i1){
                        op_shift_right{}(i1, pos);
                });
        });

        all_singleton_set_pairs<T>(op_bitand{});
        all_singleton_set_pairs<T>(op_bitor{});
        all_singleton_set_pairs<T>(op_xor{});
        all_singleton_set_pairs<T>(op_minus{});
}

BOOST_AUTO_TEST_SUITE_END()
