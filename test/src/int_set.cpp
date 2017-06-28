//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

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
#include <limits>                                       // digits

BOOST_AUTO_TEST_SUITE(Builtin)

template<class T> constexpr auto mask_none =  static_cast<T>(0);
template<class T> constexpr auto mask_one  =  static_cast<T>(1);
template<class T> constexpr auto mask_all  = ~static_cast<T>(0);

// the gcc intrinsics are defined for the following types
using UnsignedIntegerTypes = boost::mpl::vector
<
        unsigned,
        unsigned long,
        unsigned long long,
        __uint128_t
>;

using namespace xstd::builtin;

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(ctznz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(ctznz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(clznz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(clznz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popcount, T, UnsignedIntegerTypes)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = ~(mask_all<T> << i);
                BOOST_CHECK_EQUAL(popcount(b), i);
        }

        BOOST_CHECK_EQUAL(popcount(mask_all<T>), std::numeric_limits<T>::digits);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForwardNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsfnz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsfnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverseNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsrnz(mask_all<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsrnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(ctz(mask_none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(ctz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(ctz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(clz(mask_none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(clz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(clz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForward, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsf(mask_none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(bsf(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsf(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverse, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsr(mask_none<T>), -1);
        BOOST_CHECK_EQUAL(bsr(mask_all<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsr(b), i);
        }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(IntSet)

using namespace xstd;

using SetTypes = boost::mpl::vector
<
        int_set<  0>,
        int_set<  1>,
        int_set< 64>,
        int_set< 65>,
        int_set<128>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Exhaustive, T, SetTypes)
{
        auto constexpr N = T::max_size();

        prim::cons_default<N>();

        bitand_assign<N>();
        bitor_assign<N>();
        xor_assign<N>();
        minus_assign<N>();
        shift_left_assign<N>();
        shift_right_assign<N>();
        fill<N>();
        insert<N>();
        clear<N>();
        erase<N>();
        compl_<N>();
        flip_all<N>();
        flip_one<N>();

        size_<N>();
        non_member_size<N>();
        max_size<N>();
        equal_to<N>();
        not_equal_to<N>();
        less<N>();
        greater<N>();
        greater_equal<N>();
        less_equal<N>();
        is_subset_of_<N>();
        is_superset_of_<N>();
        is_proper_subset_of_<N>();
        is_proper_superset_of_<N>();
        test<N>();
        full<N>();
        not_empty<N>();
        empty_<N>();
        non_member_empty<N>();
        shift_left<N>();
        shift_right<N>();

        bitand_<N>();
        bitor_<N>();
        xor_<N>();
        minus<N>();
}

using SetTypes3 = boost::mpl::vector
<
        int_set<  0>,
        int_set<  1>,
        int_set< 31>,
        int_set< 32>,
        int_set< 33>,
        int_set< 63>,
        int_set< 64>,
        int_set< 65>,
        int_set<127>,
        int_set<128>,
        int_set<129>,
        int_set<255>,
        int_set<256>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(ValueInitializationConstructsEmpty, T, SetTypes3)
{
        auto const i0 = T{};

        BOOST_CHECK(i0.empty());
        BOOST_CHECK_EQUAL(i0.size(), 0);

        BOOST_CHECK(i0.begin() == i0.end());
        BOOST_CHECK(i0.cbegin() == i0.cend());
        BOOST_CHECK(i0.rbegin() == i0.rend());
        BOOST_CHECK(i0.crbegin() == i0.crend());

        BOOST_CHECK(begin(i0) == end(i0));
        BOOST_CHECK(cbegin(i0) == cend(i0));
        BOOST_CHECK(rbegin(i0) == rend(i0));
        BOOST_CHECK(crbegin(i0) == crend(i0));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DereferencingSingletBeginIteratorEqualsIndex, T, SetTypes3)
{
        constexpr auto N = T::max_size();
        for (auto i = 0; i < N; ++i) {
                auto const i1 = T{i};
                auto const it = i1.begin();
                auto const ref = *it;

                BOOST_CHECK(&ref == it);
                BOOST_CHECK(ref == i);
        }
}

/*
using SetTypes2 = boost::mpl::vector
<
        int_set< 50>,   // draughts
        int_set< 64>,   // chess
        int_set< 81>,   // Shogi
        int_set< 90>,   // XiangQi
        int_set<361>    // Go
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IteratorPairConstructorListInitializes, T, SetTypes2)
{
        constexpr int a[] = { 0, 1, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b = T(std::begin(a), std::end(a));

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InitializerListConstructorListInitializes, T, SetTypes2)
{
        constexpr int a[] =  { 0, 1, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b  = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };

        BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(a), std::end(a), begin(b), end(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DistanceBeginEndEqualsSize, T, SetTypes2)
{
        // TODO: in C++17, std::distance will be constexpr
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };

        BOOST_CHECK_EQUAL(std::distance(  begin(b),   end(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance( cbegin(b),  cend(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance( rbegin(b),  rend(b)), b.size());
        BOOST_CHECK_EQUAL(std::distance(crbegin(b), crend(b)), b.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsSorted, T, SetTypes2)
{
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };
        using boost::adaptors::reversed;

        BOOST_CHECK(boost::is_sorted(b           , std::less   <>{}));
        BOOST_CHECK(boost::is_sorted(b | reversed, std::greater<>{}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsStrictlyIncreasing, T, SetTypes2)
{
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };
        using boost::adaptors::reversed;

        BOOST_CHECK(boost::adjacent_find(b           , std::greater_equal<>{}) == boost:: end(b));
        BOOST_CHECK(boost::adjacent_find(b | reversed, std::   less_equal<>{}) == boost::rend(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ForwardIterationTraversesRange, T, SetTypes2)
{
        // arrange
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };

        // act
        auto it = begin(b); for (; it != end(b); ++it) {}

        // assert
        BOOST_CHECK(it == rbegin(b).base());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BackwardIterationTraversesRange, T, SetTypes2)
{
        // arrange
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };

        // act
        auto it = end(b); for (; it != begin(b); --it) {}

        // assert
        BOOST_CHECK(it == rend(b).base());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ReverseForwardIterationTraversesRange, T, SetTypes2)
{
        // arrange
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };

        // act
        auto it = rbegin(b); for (; it != rend(b); ++it) {}

        // assert
        BOOST_CHECK(it.base() == begin(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ReverseBackwardIterationTraversesRange, T, SetTypes2)
{
        // arrange
        auto const b = T{ 0, 1, T::max_size() - 2, T::max_size() - 1 };

        // act
        auto it = rend(b); for (; it != rbegin(b); --it) {}

        // assert
        BOOST_CHECK(it.base() == end(b));
}
*/
BOOST_AUTO_TEST_SUITE_END()
