#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>             // int_set
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance
#include <type_traits>                  // is_same_v

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

struct op_compl
{
        template<int N>
        constexpr auto operator()(int_set<N> const& a) const noexcept
        {
                // [bitset.members]/21
                auto expected = a;
                expected.toggle();
                BOOST_CHECK(~a == expected);

                BOOST_CHECK(~a == set_complement(a));
                BOOST_CHECK(~(~a) == a);                                // idempotent
        }

        template<int N>
        constexpr auto operator()(int_set<N> const& a, int_set<N> const& b) const noexcept
        {
                // De Morgan's Laws
                BOOST_CHECK(~(a | b) == (~a & ~b));
                BOOST_CHECK(~(a & b) == (~a | ~b));
        }
};

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

        next.flip();
        BOOST_CHECK(next == prev);
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

        next.flip(static_cast<std::size_t>(pos));
        BOOST_CHECK(next == prev);
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
        BOOST_CHECK_EQUAL(is.count(), count(is));
}

template<template<int> class IntSet, int N>
auto for_each_(IntSet<N> const& is) noexcept
{
        auto expected = 0;
        for_each(is, [&](auto) { ++expected; });
        BOOST_CHECK_EQUAL(is.count(), expected);

        if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                BOOST_CHECK_EQUAL(is.count(), std::distance(is.begin(), is.end()));
                BOOST_CHECK_EQUAL(is.count(), std::distance(is.cbegin(), is.cend()));
                BOOST_CHECK_EQUAL(is.count(), std::distance(begin(is), end(is)));
                BOOST_CHECK_EQUAL(is.count(), std::distance(cbegin(is), cend(is)));
        }
}

template<template<int> class IntSet, int N>
auto reverse_for_each_(IntSet<N> const& is) noexcept
{
        auto expected = 0;
        reverse_for_each(is, [&](auto) { ++expected; });
        BOOST_CHECK_EQUAL(is.count(), expected);

        if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                BOOST_CHECK_EQUAL(is.count(), std::distance(is.rbegin(), is.rend()));
                BOOST_CHECK_EQUAL(is.count(), std::distance(is.crbegin(), is.crend()));
                BOOST_CHECK_EQUAL(is.count(), std::distance(rbegin(is), rend(is)));
                BOOST_CHECK_EQUAL(is.count(), std::distance(crbegin(is), crend(is)));
        }
}

struct size
{
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/35
                BOOST_CHECK_EQUAL(is.size(), N);

                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(is.size(), is.max_size());
                }
        }
};

struct op_equal_to
{
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/36
                auto expected = true;
                for (auto i = 0; i < N; ++i) {
                        expected &= lhs.contains(i) == rhs.contains(i);
                }
                BOOST_CHECK_EQUAL(lhs == rhs, expected);
        }
};

struct op_not_equal_to
{
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/37
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
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
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
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(lhs, rhs), is_subset_of(rhs, lhs));
        }
};

struct is_proper_subset_of_
{
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(lhs, rhs), is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs));
        }
};

struct is_proper_superset_of_
{
        template<template<int> class IntSet, int N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(lhs, rhs), is_proper_subset_of(rhs, lhs));
        }
};

template<int N>
auto test(int const pos) noexcept
{
        // [bitset.members]/39
        auto is = int_set<N>{};
        is.insert(pos);
        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK( is.test(static_cast<std::size_t>(i)));
                } else {
                        BOOST_CHECK(!is.test(static_cast<std::size_t>(i)));
                }
        }
}

template<template<int> class IntSet, int N>
auto all(IntSet<N> const& is) noexcept
{
        // [bitset.members]/40
        BOOST_CHECK_EQUAL(is.all(), is.count() == is.size());

        if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                BOOST_CHECK_EQUAL(is.all(), is.full());
        }
}

template<template<int> class IntSet, int N>
auto any(IntSet<N> const& is) noexcept
{
        // [bitset.members]/41
        BOOST_CHECK_EQUAL(is.any(), is.count() != 0);

        if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                BOOST_CHECK_EQUAL(is.any(), !is.empty());
        }
}

template<template<int> class IntSet, int N>
auto none(IntSet<N> const& is) noexcept
{
        // [bitset.members]/42
        BOOST_CHECK_EQUAL(is.none(), is.count() == 0);

        if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                BOOST_CHECK_EQUAL(is.none(), is.empty());
                BOOST_CHECK_EQUAL(is.none(), empty(is));
        }
}

template<template<int> class IntSet, int N>
auto op_shift_left(IntSet<N> const& is, int n)
{
        // [bitset.members]/43
        auto expected = is;
        expected <<= n;
        BOOST_CHECK(is << n == expected);
}

template<template<int> class IntSet, int N>
auto op_shift_right(IntSet<N> const& is, int n)
{
        // [bitset.members]/44
        auto expected = is;
        expected >>= n;
        BOOST_CHECK(is >> n == expected);
}

// 20.9.4 bitset operators [bitset.operators]

struct op_bitand
{
        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a & a) == a);                              // idempotent
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b) const noexcept
        {
                // [bitset.operators]/1
                auto expected = a;
                expected &= b;
                BOOST_CHECK((a & b) == expected);

                BOOST_CHECK((a & b) == set_intersection(a, b));
                BOOST_CHECK((a & b) == (b & a));                        // commutative
                BOOST_CHECK(is_subset_of(a & b, a));
                BOOST_CHECK(is_subset_of(a & b, b));
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b, IntSet<N> const& c) const noexcept
        {
                BOOST_CHECK(((a & b) & c) == (a & (b & c)));            // associative
                BOOST_CHECK((a & (b | c)) == ((a & b) | (a & c)));      // distributive
        }
};

struct op_bitor
{
        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a | a) == a);                              // idempotent
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b) const noexcept
        {
                // [bitset.operators]/2
                auto expected = a;
                expected |= b;
                BOOST_CHECK((a | b) == expected);

                BOOST_CHECK((a | b) == set_union(a, b));
                BOOST_CHECK((a | b) == (b | a));                        // commutative
                BOOST_CHECK(is_subset_of(a, a | b));
                BOOST_CHECK(is_subset_of(b, a | b));
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b, IntSet<N> const& c) const noexcept
        {
                BOOST_CHECK(((a | b) | c) == (a | (b | c)));            // associative
                BOOST_CHECK((a | (b & c)) == ((a | b) & (a | c)));      // distributive
        }
};

struct op_xor
{
        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a ^ a).none());                            // nilpotent
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b) const noexcept
        {
                // [bitset.operators]/3
                auto expected = a;
                expected ^= b;
                BOOST_CHECK((a ^ b) == expected);

                BOOST_CHECK((a ^ b) == set_symmetric_difference(a, b));
                BOOST_CHECK((a ^ b) == (b ^ a));                        // commutative
                BOOST_CHECK((a ^ b) == ((a - b) | (b - a)));
                BOOST_CHECK((a ^ b) == (a | b) - (a & b));
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b, IntSet<N> const& c) const noexcept
        {
                BOOST_CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));            // associative
                BOOST_CHECK((a & (b ^ c)) == ((a & b) ^ (a & c)));      // distributive
        }
};

struct op_minus
{
        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a - a).none());                            // nilpotent
        }

        template<template<int> class IntSet, int N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b) const noexcept
        {
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        auto expected = a;
                        expected -= b;
                        BOOST_CHECK(a - b == expected);
                }

                BOOST_CHECK(a - b == set_difference(a, b));
                BOOST_CHECK(a - b == (a & ~b));
                BOOST_CHECK(a - b == (a | b) - b);
                BOOST_CHECK(a - b == a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(disjoint(a - b, b));
        }
};

}       // namespace prim
}       // namespace xstd
