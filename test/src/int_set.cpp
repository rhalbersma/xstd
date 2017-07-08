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
#include <boost/test/test_case_template.hpp>            // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>                     // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_AUTO_TEST_SUITE_END
#include <functional>                                   // greater, greater_equal, less, less_equal
#include <initializer_list>                             // initializer_list
#include <iterator>                                     // begin, end, cbegin, cend, distance

BOOST_AUTO_TEST_SUITE(IntSet)

using namespace xstd;

using SetTypes = boost::mpl::vector
<
        int_set<  0>,
        int_set<  1>,
        int_set< 64>,
        int_set< 65>,
        int_set<128>,
        int_set<129>,
        int_set<255>,
        int_set<256>
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
        auto i0 = T{};
        auto const ci0 = i0;

        BOOST_CHECK(i0.empty());
        BOOST_CHECK_EQUAL(i0.size(), 0);

        BOOST_CHECK(i0.begin() == i0.end());
        BOOST_CHECK(ci0.begin() == ci0.end());
        BOOST_CHECK(i0.rbegin() == i0.rend());
        BOOST_CHECK(ci0.rbegin() == ci0.rend());
        BOOST_CHECK(i0.cbegin() == i0.cend());
        BOOST_CHECK(i0.crbegin() == i0.crend());

        BOOST_CHECK(begin(i0) == end(i0));
        BOOST_CHECK(begin(ci0) == end(ci0));
        BOOST_CHECK(rbegin(i0) == rend(i0));
        BOOST_CHECK(rbegin(ci0) == rend(ci0));
        BOOST_CHECK(cbegin(i0) == cend(i0));
        BOOST_CHECK(crbegin(i0) == crend(i0));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IncrementingSingletBeginIteratorEqualsEndIterator, T, SetTypes3)
{
        constexpr auto N = T::max_size();
        for (auto i = 0; i < N; ++i) {
                auto i1 = T{i};
                auto const ci1 = i1;

                BOOST_CHECK(i1.size() == 1);

                BOOST_CHECK(++i1.begin() == i1.end());
                BOOST_CHECK(++ci1.begin() == ci1.end());
                BOOST_CHECK(++i1.rbegin() == i1.rend());
                BOOST_CHECK(++ci1.rbegin() == ci1.rend());
                BOOST_CHECK(++i1.cbegin() == i1.cend());
                BOOST_CHECK(++i1.crbegin() == i1.crend());

                BOOST_CHECK(++begin(i1) == end(i1));
                BOOST_CHECK(++begin(ci1) == end(ci1));
                BOOST_CHECK(++rbegin(i1) == rend(i1));
                BOOST_CHECK(++rbegin(ci1) == rend(ci1));
                BOOST_CHECK(++cbegin(i1) == cend(i1));
                BOOST_CHECK(++crbegin(i1) == crend(i1));

                { auto first = i1.begin(); first++; BOOST_CHECK(first == i1.end()); }
                { auto first = ci1.begin(); first++; BOOST_CHECK(first == ci1.end()); }
                { auto first = i1.rbegin(); first++; BOOST_CHECK(first == i1.rend()); }
                { auto first = ci1.rbegin(); first++; BOOST_CHECK(first == ci1.rend()); }
                { auto first = i1.cbegin(); first++; BOOST_CHECK(first == i1.cend()); }
                { auto first = i1.crbegin(); first++; BOOST_CHECK(first == i1.crend()); }

                { auto first = begin(i1); first++; BOOST_CHECK(first == end(i1)); }
                { auto first = begin(ci1); first++; BOOST_CHECK(first == end(ci1)); }
                { auto first = rbegin(i1); first++; BOOST_CHECK(first == rend(i1)); }
                { auto first = rbegin(ci1); first++; BOOST_CHECK(first == rend(ci1)); }
                { auto first = cbegin(i1); first++; BOOST_CHECK(first == cend(i1)); }
                { auto first = crbegin(i1); first++; BOOST_CHECK(first == crend(i1)); }
        }

        for (auto i = 0; i < N; ++i) {
                for (auto j = i + 1; j < N; ++j) {
                        auto const is2 = T{i, j};
                        auto const cis2 = is2;

                        for (auto first = is2.begin(), last = is2.end(); first != last; ++first) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto first = cis2.begin(), last = cis2.end(); first != last; ++first) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto first = is2.rbegin(), last = is2.rend(); first != last; ++first) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto first = cis2.rbegin(), last = cis2.rend(); first != last; ++first) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto first = is2.cbegin(), last = is2.cend(); first != last; ++first) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto first = is2.crbegin(), last = is2.crend(); first != last; ++first) { BOOST_CHECK_EQUAL(true, true); }
                }
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DecrementingSingletEndIteratorEqualsBeginIterator, T, SetTypes3)
{
        constexpr auto N = T::max_size();
        for (auto i = 0; i < N; ++i) {
                auto i1 = T{i};
                auto const ci1 = i1;

                BOOST_CHECK(i1.size() == 1);

                BOOST_CHECK(--i1.end() == i1.begin());
                BOOST_CHECK(--ci1.end() == ci1.begin());
                BOOST_CHECK(--i1.rend() == i1.rbegin());
                BOOST_CHECK(--ci1.rend() == ci1.rbegin());
                BOOST_CHECK(--i1.cend() == i1.cbegin());
                BOOST_CHECK(--i1.crend() == i1.crbegin());

                BOOST_CHECK(--end(i1) == begin(i1));
                BOOST_CHECK(--end(ci1) == begin(ci1));
                BOOST_CHECK(--rend(i1) == rbegin(i1));
                BOOST_CHECK(--rend(ci1) == rbegin(ci1));
                BOOST_CHECK(--cend(i1) == cbegin(i1));
                BOOST_CHECK(--crend(i1) == crbegin(i1));

                { auto last = i1.end(); last--; BOOST_CHECK(last == i1.begin()); }
                { auto last = ci1.end(); last--; BOOST_CHECK(last == ci1.begin()); }
                { auto last = i1.rend(); last--; BOOST_CHECK(last == i1.rbegin()); }
                { auto last = ci1.rend(); last--; BOOST_CHECK(last == ci1.rbegin()); }
                { auto last = i1.cend(); last--; BOOST_CHECK(last == i1.cbegin()); }
                { auto last = i1.crend(); last--; BOOST_CHECK(last == i1.crbegin()); }

                { auto last = end(i1); last--; BOOST_CHECK(last == begin(i1)); }
                { auto last = end(ci1); last--; BOOST_CHECK(last == begin(ci1)); }
                { auto last = rend(i1); last--; BOOST_CHECK(last == rbegin(i1)); }
                { auto last = rend(ci1); last--; BOOST_CHECK(last == rbegin(ci1)); }
                { auto last = cend(i1); last--; BOOST_CHECK(last == cbegin(i1)); }
                { auto last = crend(i1); last--; BOOST_CHECK(last == crbegin(i1)); }
        }

        for (auto i = 0; i < N; ++i) {
                for (auto j = i + 1; j < N; ++j) {
                        auto const is2 = T{i, j};
                        auto const cis2 = is2;

                        for (auto last = is2.end(), first = is2.begin(); last != first; --last) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto last = cis2.end(), first = cis2.begin(); last != first; --last) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto last = is2.rend(), first = is2.rbegin(); last != first; --last) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto last = cis2.rend(), first = cis2.rbegin(); last != first; --last) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto last = is2.cend(), first = is2.cbegin(); last != first; --last) { BOOST_CHECK_EQUAL(true, true); }
                        for (auto last = is2.crend(), first = is2.crbegin(); last != first; --last) { BOOST_CHECK_EQUAL(true, true); }
                }
        }
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
