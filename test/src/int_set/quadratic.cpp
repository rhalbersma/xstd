//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/exhaustive.hpp>
#include <int_set/primitive.hpp>
#include <xstd/bitset.hpp>
#include <xstd/int_set.hpp>
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Quadratic)

using namespace xstd;

using SetTypes = boost::mpl::vector
<
        std::bitset<32>,
        int_set<32>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        all_pairs<T>(test::const_reference{});
        all_pairs<T>(test::const_iterator{});
        all_pairs<T>(test::for_each_{});
        all_pairs<T>(test::reverse_for_each_{});

        all_singlet_pairs<T>(test::op_bitand_assign{});
        all_singlet_pairs<T>(test::op_bitor_assign{});
        all_singlet_pairs<T>(test::op_xor_assign{});
        all_singlet_pairs<T>(test::op_minus_assign{});

        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        test::op_shift_left_assign{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        test::op_shift_left_assign{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        test::op_shift_right_assign{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        test::op_shift_right_assign{}(i1, pos);
                });
        });

        all_singlet_pairs<T>(test::op_compl{});

        all_singlet_pairs<T>(test::op_equal_to{});
        all_singlet_pairs<T>(test::op_not_equal_to{});

        all_singlet_pairs<T>(test::op_less{});
        all_singlet_pairs<T>(test::op_greater{});
        all_singlet_pairs<T>(test::op_greater_equal{});
        all_singlet_pairs<T>(test::op_less_equal{});

        all_singlet_pairs<T>(test::is_subset_of_{});
        all_singlet_pairs<T>(test::is_superset_of_{});
        all_singlet_pairs<T>(test::is_proper_subset_of_{});
        all_singlet_pairs<T>(test::is_proper_superset_of_{});

        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        test::op_shift_left{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        test::op_shift_left{}(i1, pos);
                });
        });

        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        test::op_shift_right{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        test::op_shift_right{}(i1, pos);
                });
        });

        all_singlet_pairs<T>(test::op_bitand{});
        all_singlet_pairs<T>(test::op_bitor{});
        all_singlet_pairs<T>(test::op_xor{});
        all_singlet_pairs<T>(test::op_minus{});
}

BOOST_AUTO_TEST_SUITE_END()
