//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>                               // bitset
#include <xstd/int_set.hpp>                     // int_set
#include <int_set/deprecated/bitset.hpp>        // bitset
#include <int_set/deprecated/int_set.hpp>       // int_set
#include <int_set/exhaustive.hpp>
#include <int_set/primitive.hpp>
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using SetTypes = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset< 32>
,       std::bitset< 64>
,       std::bitset< 96>
,       std::bitset<128>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set<  2, uint32_t>
,       int_set< 32, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 34, uint32_t>
,       int_set< 64, uint32_t>
,       int_set< 65, uint32_t>
,       int_set< 66, uint32_t>
,       int_set< 96, uint32_t>
,       int_set< 64, uint64_t>
,       int_set<128, uint64_t>
#if defined(__GNUG__)
,       int_set<128, __uint128_t>
,       int_set<256, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        all_singleton_arrays<T>([](auto const& a1) {
                constructor<T>{}(a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                constructor<T>{}(ilist1);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                mem_assign{}(~T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                mem_assign{}(~T{}, ilist1);
        });

        all_cardinality_sets<T>(const_reference{});
        all_singleton_sets<T>(const_reference{});

        all_cardinality_sets<T>(const_iterator{});
        all_singleton_sets<T>(const_iterator{});

        all_cardinality_sets<T>(mem_front{});
        all_singleton_sets<T>(mem_front{});

        all_cardinality_sets<T>(mem_back{});
        all_singleton_sets<T>(mem_back{});

        all_cardinality_sets<T>([](auto const& in) {
                mem_accumulate{}(in, 0);
        });
        all_singleton_sets<T>([](auto const& i1) {
                mem_accumulate{}(i1, 0);
        });

        all_cardinality_sets<T>(mem_for_each{});
        all_singleton_sets<T>(mem_for_each{});

        all_cardinality_sets<T>(mem_reverse_for_each{});
        all_singleton_sets<T>(mem_reverse_for_each{});

        all_singleton_sets<T>(fn_set{});                        // implementation of fill() branches on excess_bits, and on num_blocks >= 3
        all_values<T>([](auto const pos) {
                fn_set{}(T{}, pos);
                fn_set{}(T{}, pos, true);
                fn_set{}(T{}, pos, false);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                mem_insert{}(T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                mem_insert{}(T{}, ilist1);
        });

        all_singleton_sets<T>(fn_reset{});                      // implementation of clear() branches on num_blocks >= 3
        all_values<T>([](auto const pos) {
                fn_reset{}(~T{}, pos);
        });

        all_singleton_arrays<T>([](auto const& a1) {
                mem_erase{}(~T{}, a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                mem_erase{}(T{}, ilist1);
        });

        all_singleton_sets<T>(op_compl{});
        all_singleton_sets<T>(fn_flip{});                       // implementation of toggle() branches on excess_bits, and on num_blocks >= 3
        all_values<T>([](auto const pos) {
                fn_flip{}(T{}, pos);
        });

        all_cardinality_sets<T>(mem_count{});                   // implementation of count() branches on num_blocks >= 3
        all_singleton_sets<T>(fn_size{});

        all_cardinality_sets<T>(op_equal_to{});
        all_cardinality_sets<T>(op_less{});
        all_cardinality_sets<T>(fn_intersects{});
        all_cardinality_sets<T>(fn_is_subset_of{});

        all_singleton_sets<T>(op_equal_to{});
        all_singleton_sets<T>(op_not_equal_to{});

        all_singleton_sets<T>(op_less{});
        all_singleton_sets<T>(op_greater{});
        all_singleton_sets<T>(op_greater_equal{});
        all_singleton_sets<T>(op_less_equal{});

        all_singleton_sets<T>(fn_intersects{});
        all_singleton_sets<T>(fn_disjoint{});

        all_singleton_sets<T>(fn_is_subset_of{});
        all_singleton_sets<T>(fn_is_superset_of{});
        all_singleton_sets<T>(fn_is_proper_subset_of{});
        all_singleton_sets<T>(fn_is_proper_superset_of{});

        all_values<T>([](auto const pos) {
                fn_test{}(T{}, pos);
        });

        all_cardinality_sets<T>(fn_all{});                      // implementation of full() branches on excess_bits, and on num_blocks >= 3
        all_cardinality_sets<T>(fn_any{});
        all_cardinality_sets<T>(fn_none{});                     // implementation of empty() branches on num_blocks >= 3

        all_values<T>([](auto const pos) {
                op_at{}(T{}, pos);
        });

        all_singleton_sets<T>(op_bitand{});
        all_singleton_sets<T>(op_bitor{});
        all_singleton_sets<T>(op_xor{});
        all_singleton_sets<T>(op_minus{});
}

BOOST_AUTO_TEST_SUITE_END()
