#include <int_set/exhaustive.hpp>
#include <xstd/int_set.hpp>                     // int_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_AUTO_TEST_SUITE_END
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE

namespace xstd {

BOOST_AUTO_TEST_SUITE(BitSet)

using SetTypes = boost::mpl::vector
<
        int_set<  0>,
        int_set<  1>,
        int_set< 63>,
        int_set< 64>,
        int_set< 65>,
        int_set<127>,
        int_set<128>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        auto constexpr N = T::max_size();

        // constructors

        BOOST_CHECK(cons_default<N>());

        // comparators

        BOOST_CHECK(op_equal_to<N>());
        BOOST_CHECK(op_not_equal_to<N>());
        BOOST_CHECK(op_less<N>());
        BOOST_CHECK(op_subset_of<N>());
        BOOST_CHECK(op_proper_subset_of<N>());

        // modifiers

        BOOST_CHECK(set_one<N>());
        BOOST_CHECK(reset_one<N>());
        BOOST_CHECK(flip_one<N>());

        BOOST_CHECK(set_all<N>());
        BOOST_CHECK(reset_all<N>());
        BOOST_CHECK(flip_all<N>());

        BOOST_CHECK(op_and_assign<N>());
        BOOST_CHECK(op_or_assign<N>());
        BOOST_CHECK(op_xor_assign<N>());
        BOOST_CHECK(op_minus_assign<N>());
        BOOST_CHECK(op_shift_left_assign<N>());
        BOOST_CHECK(op_shift_right_assign<N>());

        // observers

        BOOST_CHECK(all<N>());
        BOOST_CHECK(any<N>());
        BOOST_CHECK(none<N>());
        BOOST_CHECK(count<N>());

        // operators

        BOOST_CHECK(op_complement<N>());
        BOOST_CHECK(op_and<N>());
        BOOST_CHECK(op_or<N>());
        BOOST_CHECK(op_xor<N>());
        BOOST_CHECK(op_minus<N>());
        BOOST_CHECK(op_shift_left<N>());
        BOOST_CHECK(op_shift_right<N>());
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace xstd
