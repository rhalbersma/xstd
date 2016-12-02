#include <int_set/exhaustive.hpp>
#include <xstd/int_set.hpp>                     // int_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_AUTO_TEST_SUITE_END
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <algorithm>                            // adjacent_find, is_sorted, minmax_element
#include <functional>                           // greater, greater_equal, less, less_equal
#include <initializer_list>                     // initializer_list
#include <iterator>                             // begin, end, cbegin, cend, distance
#include <tuple>                                // tie

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/adjacent_find.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>
#include <boost/range/end.hpp>

namespace xstd {

BOOST_AUTO_TEST_SUITE(BitSet)

using SetTypes = boost::mpl::vector
<
        int_set< 18>,
        int_set< 32>,
        int_set< 50>,
        int_set< 64>,
        int_set< 72>,
        int_set< 81>,
        int_set< 90>,
        int_set<128>,
        int_set<256>,
        int_set<361>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(DefaultConstructorZeroInitializes, T, SetTypes)
{
        T b{};

        BOOST_CHECK(b.none());
        BOOST_CHECK(b.count() == 0);
        BOOST_CHECK(  begin(b) ==   end(b));
        BOOST_CHECK( cbegin(b) ==  cend(b));
        BOOST_CHECK( rbegin(b) ==  rend(b));
        BOOST_CHECK(crbegin(b) == crend(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IteratorPairConstructorListInitializes, T, SetTypes)
{
        constexpr int a[] = { 0, 1, 2, T::size() - 2, T::size() - 1 };
        auto b = T(std::begin(a), std::end(a));

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InitializerListConstructorListInitializes, T, SetTypes)
{
        constexpr int a[]  =  { 0, 1, 2, T::size() - 2, T::size() - 1 };
        constexpr auto b = T{ 0, 1, 2, T::size() - 2, T::size() - 1 };

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DistanceBeginEndEqualsSize, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::size() - 2, T::size() - 1 };

        BOOST_CHECK_EQUAL(std::distance(  begin(b),   end(b)), b.count());
        BOOST_CHECK_EQUAL(std::distance( cbegin(b),  cend(b)), b.count());
        BOOST_CHECK_EQUAL(std::distance( rbegin(b),  rend(b)), b.count());
        BOOST_CHECK_EQUAL(std::distance(crbegin(b), crend(b)), b.count());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsSorted, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::size() - 2, T::size() - 1 };

        BOOST_CHECK(boost::is_sorted(b                            , std::less<>{})   );
        BOOST_CHECK(boost::is_sorted(b | boost::adaptors::reversed, std::greater<>{}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsStrictlyIncreasing, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::size() - 2, T::size() - 1 };

        BOOST_CHECK(boost::adjacent_find(b                            , std::greater_equal<>{}) == boost::end(b) );
        BOOST_CHECK(boost::adjacent_find(b | boost::adaptors::reversed, std::less_equal<>{})    == boost::rend(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        auto constexpr N = T::size();

        // constructors

        BOOST_CHECK(constructor_default<N>());

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
