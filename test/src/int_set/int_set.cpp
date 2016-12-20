#include <int_set/exhaustive.hpp>
#include <xstd/int_set.hpp>                     // int_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_AUTO_TEST_SUITE_END
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE

namespace xstd {

BOOST_AUTO_TEST_SUITE(IntSet)

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

        BOOST_CHECK(cons_default<N>());

        BOOST_CHECK(bitand_assign<N>());
        BOOST_CHECK(bitor_assign<N>());
        BOOST_CHECK(xor_assign<N>());
        BOOST_CHECK(minus_assign<N>());
        BOOST_CHECK(shift_left_assign<N>());
        BOOST_CHECK(shift_right_assign<N>());
        BOOST_CHECK(fill<N>());
        BOOST_CHECK(insert<N>());
        BOOST_CHECK(clear<N>());
        BOOST_CHECK(erase<N>());
        BOOST_CHECK(compl_<N>());
        BOOST_CHECK(flip_all<N>());
        BOOST_CHECK(flip_one<N>());

        BOOST_CHECK(size_<N>());
        BOOST_CHECK(max_size<N>());
        BOOST_CHECK(equal_to<N>());
        BOOST_CHECK(not_equal_to<N>());
        BOOST_CHECK(less<N>());
        BOOST_CHECK(is_subset_of_<N>());
        BOOST_CHECK(is_proper_subset_of_<N>());
        BOOST_CHECK(test<N>());
        BOOST_CHECK(full<N>());
        BOOST_CHECK(not_empty<N>());
        BOOST_CHECK(empty_<N>());
        BOOST_CHECK(shift_left<N>());
        BOOST_CHECK(shift_right<N>());

        BOOST_CHECK(bitand_<N>());
        BOOST_CHECK(bitor_<N>());
        BOOST_CHECK(xor_<N>());
        BOOST_CHECK(minus<N>());
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace xstd
