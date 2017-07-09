#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>             // int_set
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance

namespace xstd {
namespace prim {

// 20.9.1 bitset constructors [bitset.cons]

// [bitset.cons]/1
template<int N>
constexpr auto cons_default() noexcept
{
        BOOST_CHECK(int_set<N>{}.empty());
}

// 20.9.2 bitset members [bitset.members]

// [bitset.members]/1
struct op_bitand_assign
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = lhs;

                next &= rhs;

                for (auto i = 0; i < N; ++i) {
                        if (!rhs.contains(i)) {
                                BOOST_CHECK(!next.contains(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.contains(i), prev.contains(i));
                        }
                }
        }
};

// [bitset.members]/3
struct op_bitor_assign
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = lhs;

                next |= rhs;

                for (auto i = 0; i < N; ++i) {
                        if (rhs.contains(i)) {
                                BOOST_CHECK(next.contains(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.contains(i), prev.contains(i));
                        }
                }
        }
};

// [bitset.members]/5
struct op_xor_assign
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = lhs;

                next ^= rhs;

                for (auto i = 0; i < N; ++i) {
                        if (rhs.contains(i)) {
                                BOOST_CHECK_EQUAL(next.contains(i), !prev.contains(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.contains(i),  prev.contains(i));
                        }
                }
        }
};

struct op_minus_assign
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = lhs;

                next -= rhs;

                for (auto i = 0; i < N; ++i) {
                        if (rhs.contains(i)) {
                                BOOST_CHECK(!next.contains(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.contains(i), prev.contains(i));
                        }
                }
        }
};

// [bitset.members]/7
template<int N>
auto op_shift_left_assign(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = is;

        next <<= pos;

        for (auto I = 0; I < N; ++I) {
                if (I < pos) {
                        BOOST_CHECK(!next.contains(I));
                } else {
                        BOOST_CHECK_EQUAL(next.contains(I), prev.contains(I - pos));
                }
        }
}

// [bitset.members]/9
template<int N>
auto op_shift_right_assign(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = is;

        next >>= pos;

        for (auto I = 0; I < N; ++I) {
                if (pos >= N - I) {
                        BOOST_CHECK(!next.contains(I));
                } else {
                        BOOST_CHECK_EQUAL(next.contains(I), prev.contains(I + pos));
                }
        }
}

// [bitset.members]/11
template<int N>
auto fill(int_set<N> const& is) noexcept
{
        auto value = is;

        value.fill();

        BOOST_CHECK(value.full());
}

// [bitset.members]/14
template<int N>
constexpr auto insert(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = prev;

        next.insert(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK(next.contains(i));
                } else {
                        BOOST_CHECK_EQUAL(next.contains(i), prev.contains(i));
                }
        }
}

// [bitset.members]/16
template<int N>
auto clear(int_set<N> const& is) noexcept
{
        auto value = is;

        value.clear();

        BOOST_CHECK(value.empty());
}

// [bitset.members]/19
template<int N>
constexpr auto erase(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = prev;

        next.erase(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK(!next.contains(i));
                } else {
                        BOOST_CHECK_EQUAL(next.contains(i), prev.contains(i));
                }
        }
}

template<int N, class InputIterator>
constexpr auto erase(int_set<N> const& is, InputIterator first, InputIterator last)
{
        auto const prev = is;
        auto next = prev;

        next.erase(first, last);

        while (first != last) {
                BOOST_CHECK(!next.contains(*first++));
        }
}

template<int N>
constexpr auto erase(int_set<N> const& is, std::initializer_list<int> ilist)
{
        auto const prev = is;
        auto next = prev;

        next.erase(ilist);

        for (auto&& elem : ilist) {
                BOOST_CHECK(!next.contains(elem));
        }
}

// [bitset.members]/21
template<int N>
constexpr auto op_compl(int_set<N> const& is) noexcept
{
        auto expected = is;

        expected.toggle();

        BOOST_CHECK(~is == expected);
        BOOST_CHECK(set_complement(is) == expected);
}

// [bitset.members]/23
template<int N>
constexpr auto toggle_all(int_set<N> const& is) noexcept
{
        auto const prev = is;
        auto next = is;

        next.toggle();

        for (auto i = 0; i < N; ++i) {
                BOOST_CHECK_NE(next.contains(i), prev.contains(i));
        }
}

// [bitset.members]/26
template<int N>
constexpr auto toggle_one(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = is;

        next.toggle(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK_NE   (next.contains(i), prev.contains(i));
                } else {
                        BOOST_CHECK_EQUAL(next.contains(i), prev.contains(i));
                }
        }
}

// [bitset.members]/34
template<int N>
auto count_(int_set<N> const& is) noexcept
{
        auto expected = 0;

        for (auto i = 0; i < N; ++i) {
                expected += is.contains(i);
        }

        BOOST_CHECK_EQUAL(is.count(), expected);
        BOOST_CHECK_EQUAL(count(is), is.count());
}

template<int N>
auto for_each_(int_set<N> const& is) noexcept
{
        auto expected = 0;

        is.for_each([&](auto) { ++expected; });

        BOOST_CHECK_EQUAL(is.count(), expected);
        BOOST_CHECK_EQUAL(std::distance(is.begin(), is.end()), expected);
}

template<int N>
auto reverse_for_each_(int_set<N> const& is) noexcept
{
        auto expected = 0;

        is.reverse_for_each([&](auto) { ++expected; });

        BOOST_CHECK_EQUAL(is.count(), expected);
        BOOST_CHECK_EQUAL(std::distance(is.rbegin(), is.rend()), expected);
}

// [bitset.members]/35
template<int N>
auto max_size(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(is.max_size(), N);
        BOOST_CHECK_EQUAL(is.size(), is.max_size());
}

// [bitset.members]/36
struct op_equal_to
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = true;

                for (auto i = 0; i < N; ++i) {
                        expected &= lhs.contains(i) == rhs.contains(i);
                }

                BOOST_CHECK_EQUAL(lhs == rhs, expected);
        }
};

// [bitset.members]/37
struct op_not_equal_to
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs != rhs, !(lhs == rhs));
        }
};

struct op_less
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = false;

                for (auto i = N - 1; i >= 0; --i) {
                        if (!lhs.contains(i) && rhs.contains(i)) { expected = true; break; }
                        if (!rhs.contains(i) && lhs.contains(i)) {                  break; }
                }

                BOOST_CHECK_EQUAL(lhs < rhs, expected);
        }
};

struct op_greater
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs > rhs, rhs < lhs);
        }
};

struct op_greater_equal
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
        }
};

struct op_less_equal
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs <= rhs, !(rhs < lhs));
        }
};

struct is_subset_of_
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = true;

                for (auto i = 0; i < N; ++i) {
                        expected &= !lhs.contains(i) || rhs.contains(i);
                }

                BOOST_CHECK_EQUAL(is_subset_of(lhs, rhs), expected);
        }
};

struct is_superset_of_
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(lhs, rhs), is_subset_of(rhs, lhs));
        }
};

struct is_proper_subset_of_
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(lhs, rhs), is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs));
        }
};

struct is_proper_superset_of_
{
        template<int N>
        auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(lhs, rhs), is_proper_subset_of(rhs, lhs));
        }
};

// [bitset.members]/39
template<int N>
auto contains(int const pos) noexcept
{
        auto is = int_set<N>{};

        is.insert(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK( is.contains(i));
                } else {
                        BOOST_CHECK(!is.contains(i));
                }
        }
}

// [bitset.members]/40
template<int N>
auto full(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(is.full(), is.count() == is.max_size());
        BOOST_CHECK_EQUAL(is.all(), is.full());
}

// [bitset.members]/41
template<int N>
auto not_empty(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(!is.empty(), is.count() != 0);
        BOOST_CHECK_EQUAL(is.any(), !is.empty());
}

// [bitset.members]/42
template<int N>
auto empty_(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(is.empty(), is.count() == 0);
        BOOST_CHECK_EQUAL(empty(is), is.empty());
        BOOST_CHECK_EQUAL(is.none(), is.empty());
}

// [bitset.members]/43
template<int N>
auto op_shift_left(int_set<N> const& is, int n)
{
        auto expected = is;

        expected <<= n;

        BOOST_CHECK(is << n == expected);
}

// [bitset.members]/44
template<int N>
auto op_shift_right(int_set<N> const& is, int n)
{
        auto expected = is;

        expected >>= n;

        BOOST_CHECK(is >> n == expected);
}

// 20.9.4 bitset operators [bitset.operators]

// [bitset.operators]/1
struct op_bitand
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = lhs;

                expected &= rhs;

                BOOST_CHECK((lhs & rhs) == expected);
                BOOST_CHECK(set_intersection(lhs, rhs) == expected);
        }
};

// [bitset.operators]/2
struct op_bitor
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = lhs;

                expected |= rhs;

                BOOST_CHECK((lhs | rhs) == expected);
                BOOST_CHECK(set_union(lhs, rhs) == expected);
        }
};

// [bitset.operators]/3
struct op_xor
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = lhs;

                expected ^= rhs;

                BOOST_CHECK((lhs ^ rhs) == expected);
                BOOST_CHECK(set_symmetric_difference(lhs, rhs) == expected);
        }
};

struct op_minus
{
        template<int N>
        constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
        {
                auto expected = lhs;

                expected -= rhs;

                BOOST_CHECK(lhs - rhs == expected);
                BOOST_CHECK(set_difference(lhs, rhs) == expected);
        }
};

}       // namespace prim
}       // namespace xstd
