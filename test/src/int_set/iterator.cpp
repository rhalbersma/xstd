#include <xstd/int_set.hpp>                             // int_set
#include <boost/mpl/vector.hpp>                         // vector
#include <boost/range/adaptor/reversed.hpp>             // reversed
#include <boost/range/algorithm/adjacent_find.hpp>      // adjacent_find
#include <boost/range/algorithm_ext/is_sorted.hpp>      // is_sorted
#include <boost/range/end.hpp>                          // end
#include <boost/test/unit_test.hpp>                     // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_AUTO_TEST_SUITE_END
#include <boost/test/test_case_template.hpp>            // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <functional>                                   // greater, greater_equal, less, less_equal
#include <initializer_list>                             // initializer_list
#include <iterator>                                     // begin, end, cbegin, cend, distance


namespace xstd {

BOOST_AUTO_TEST_SUITE(BitIterator)

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

BOOST_AUTO_TEST_CASE_TEMPLATE(ForwardIterationTraversesRange, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = begin(b); for (; it != end(b); ++it){}

        BOOST_CHECK(it == rbegin(b).base());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BackwardIterationTraversesRange, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = end(b); for (; it != begin(b); --it){}

        BOOST_CHECK(it == rend(b).base());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ReverseForwardIterationTraversesRange, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = rbegin(b); for (; it != rend(b); ++it){}

        BOOST_CHECK(it.base() == begin(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ReverseBackwardIterationTraversesRange, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto it = rend(b); for (; it != rbegin(b); --it){}

        BOOST_CHECK(it.base() == end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ValueInitializationYieldsEmptySet, T, SetTypes)
{
        T b{};

        BOOST_CHECK(b.empty());
        BOOST_CHECK(b.size() == 0);

        BOOST_CHECK(b.begin()   == b.end());
        BOOST_CHECK(b.cbegin()  == b.cend());
        BOOST_CHECK(b.rbegin()  == b.rend());
        BOOST_CHECK(b.crbegin() == b.crend());

        BOOST_CHECK( begin(b)  == end(b));
        BOOST_CHECK(cbegin(b)  == cend(b));
        BOOST_CHECK(rbegin(b)  == rend(b));
        BOOST_CHECK(crbegin(b) == crend(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IteratorPairConstructorListInitializes, T, SetTypes)
{
        constexpr int a[] = { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        auto b = T(std::begin(a), std::end(a));

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InitializerListConstructorListInitializes, T, SetTypes)
{
        constexpr int a[]  =  { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DistanceBeginEndEqualsSize, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };

        BOOST_CHECK_EQUAL(std::distance(  begin(b),   end(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance( cbegin(b),  cend(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance( rbegin(b),  rend(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance(crbegin(b), crend(b)), b.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsSorted, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        using boost::adaptors::reversed;

        BOOST_CHECK(boost::is_sorted(b           , std::less   <>{}));
        BOOST_CHECK(boost::is_sorted(b | reversed, std::greater<>{}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsStrictlyIncreasing, T, SetTypes)
{
        constexpr auto b = T{ 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        using boost::adaptors::reversed;

        BOOST_CHECK(boost::adjacent_find(b           , std::greater_equal<>{}) == boost:: end(b));
        BOOST_CHECK(boost::adjacent_find(b | reversed, std::   less_equal<>{}) == boost::rend(b));
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace xstd
