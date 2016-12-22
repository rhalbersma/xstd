#include <int_set/exhaustive.hpp>
#include <xstd/int_set.hpp>                             // int_set
#include <boost/mpl/vector.hpp>                         // vector
#include <boost/range/adaptor/reversed.hpp>             // reversed
#include <boost/range/algorithm/adjacent_find.hpp>      // adjacent_find
#include <boost/range/algorithm_ext/is_sorted.hpp>      // is_sorted
#include <boost/range/end.hpp>                          // end
#include <boost/test/unit_test.hpp>                     // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_AUTO_TEST_SUITE_END
#include <boost/test/test_case_template.hpp>            // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <functional>                                   // greater, greater_equal, less, less_equal
#include <initializer_list>                             // initializer_list
#include <iterator>                                     // begin, end, cbegin, cend, distance

BOOST_AUTO_TEST_SUITE(IntSet)

using namespace xstd;

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

BOOST_AUTO_TEST_CASE_TEMPLATE(ValueInitializationConstructsEmpty, T, SetTypes)
{
        T b{};

        BOOST_CHECK(b.empty());
        BOOST_CHECK_EQUAL(b.size(), 0);

        BOOST_CHECK(b.begin()   == b.end());
        BOOST_CHECK(b.cbegin()  == b.cend());
        BOOST_CHECK(b.rbegin()  == b.rend());
        BOOST_CHECK(b.crbegin() == b.crend());

        BOOST_CHECK( begin(b)  == end(b));
        BOOST_CHECK(cbegin(b)  == cend(b));
        BOOST_CHECK(rbegin(b)  == rend(b));
        BOOST_CHECK(crbegin(b) == crend(b));
}

using SetTypes2 = boost::mpl::vector
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

BOOST_AUTO_TEST_CASE_TEMPLATE(IteratorPairConstructorListInitializes, T, SetTypes2)
{
        constexpr int a[] = { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto const b = T(std::begin(a), std::end(a));

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InitializerListConstructorListInitializes, T, SetTypes2)
{
        constexpr int a[] =  { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b  = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DistanceBeginEndEqualsSize, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };

        BOOST_CHECK_EQUAL(std::distance(  begin(b),   end(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance( cbegin(b),  cend(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance( rbegin(b),  rend(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance(crbegin(b), crend(b)), b.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsSorted, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        using boost::adaptors::reversed;

        BOOST_CHECK(boost::is_sorted(b           , std::less   <>{}));
        BOOST_CHECK(boost::is_sorted(b | reversed, std::greater<>{}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsStrictlyIncreasing, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        using boost::adaptors::reversed;

        BOOST_CHECK(boost::adjacent_find(b           , std::greater_equal<>{}) == boost:: end(b));
        BOOST_CHECK(boost::adjacent_find(b | reversed, std::   less_equal<>{}) == boost::rend(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ForwardIterationTraversesRange, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = begin(b); for (; it != end(b); ++it){}

        BOOST_CHECK(it == rbegin(b).base());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BackwardIterationTraversesRange, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = end(b); for (; it != begin(b); --it){}

        BOOST_CHECK(it == rend(b).base());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ReverseForwardIterationTraversesRange, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = rbegin(b); for (; it != rend(b); ++it){}

        BOOST_CHECK(it.base() == begin(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ReverseBackwardIterationTraversesRange, T, SetTypes2)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = rend(b); for (; it != rbegin(b); --it){}

        BOOST_CHECK(it.base() == end(b));
}

BOOST_AUTO_TEST_SUITE_END()
