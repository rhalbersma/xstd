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

template<class T>
struct constructor
{
        constexpr auto operator()() const noexcept
        {
                // [bitset.cons]/1
                BOOST_CHECK(T{}.empty());
        }
};

// 20.9.2 bitset members [bitset.members]

struct op_bitand_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/1
                auto const prev = lhs;
                auto next = prev; next &= rhs;
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (!rhs.test(i)) {
                                BOOST_CHECK(!next.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                        }
                }
        }
};

struct op_bitor_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/3
                auto const prev = lhs;
                auto next = prev; next |= rhs;
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (rhs.test(i)) {
                                BOOST_CHECK(next.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                        }
                }
        }
};

struct op_xor_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/5
                auto const prev = lhs;
                auto next = prev; next ^= rhs;
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (rhs.test(i)) {
                                BOOST_CHECK_EQUAL(next.test(i), !prev.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i),  prev.test(i));
                        }
                }
        }
};

struct op_minus_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = prev; next -= rhs;
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (rhs.test(i)) {
                                BOOST_CHECK(!next.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                        }
                }
        }
};

struct op_shift_left_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                // [bitset.members]/7
                auto const prev = is;
                auto next = prev; next <<= pos;
                for (auto I = SizeType{0}; I < N; ++I) {
                        if (I < pos) {
                                BOOST_CHECK(!next.test(I));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(I), prev.test(I - pos));
                        }
                }
        }
};

struct op_shift_right_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                // [bitset.members]/9
                auto const prev = is;
                auto next = prev; next >>= pos;
                for (auto I = SizeType{0}; I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!next.test(I));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(I), prev.test(I + pos));
                        }
                }
        }
};

struct set
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/11
                auto value = is; value.set();
                BOOST_CHECK(value.all());

                // [bitset.memers]/13
                BOOST_CHECK_THROW(value.set(N), std::out_of_range);
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                // [bitset.members]/14
                auto const prev = is;
                auto next = prev; next.set(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (i == pos) {
                                BOOST_CHECK(next.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                        }
                }
        }
};

struct reset
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/16
                auto value = is; value.reset();
                BOOST_CHECK(value.none());

                // [bitset.memers]/18
                BOOST_CHECK_THROW(value.reset(N), std::out_of_range);
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                // [bitset.members]/19
                auto const prev = is;
                auto next = prev; next.reset(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (i == pos) {
                                BOOST_CHECK(!next.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                        }
                }
        }

        template<template<auto> class IntSet, auto N, class InputIterator>
        auto operator()(IntSet<N> const& is, InputIterator first, InputIterator last) const
        {
                auto const prev = is;
                auto next = prev; next.erase(first, last);
                while (first != last) {
                        BOOST_CHECK(!next.test(*first++));
                }
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& is, std::initializer_list<SizeType> ilist) const
        {
                auto const prev = is;
                auto next = prev; next.erase(ilist);
                for (auto&& elem : ilist) {
                        BOOST_CHECK(!next.test(elem));
                }
        }
};

struct op_compl
{
        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                // [bitset.members]/21
                auto expected = a;
                expected.toggle();
                BOOST_CHECK(~a == expected);

                BOOST_CHECK(~a == set_complement(a));
                BOOST_CHECK(~(~a) == a);                                // idempotent
        }

        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b) const noexcept
        {
                // De Morgan's Laws
                BOOST_CHECK(~(a | b) == (~a & ~b));
                BOOST_CHECK(~(a & b) == (~a | ~b));
        }
};

struct flip
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/23
                auto const prev = is;
                auto next = is; next.flip();
                for (auto i = SizeType{0}; i < N; ++i) {
                        BOOST_CHECK_NE(next.test(i), prev.test(i));
                }

                next.flip();
                BOOST_CHECK(next == prev);

                // [bitset.members]/25
                BOOST_CHECK_THROW(next.flip(N), std::out_of_range);
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                // [bitset.members]/26
                auto const prev = is;
                auto next = is; next.flip(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (i == pos) {
                                BOOST_CHECK_NE   (next.test(i), prev.test(i));
                        } else {
                                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                        }
                }

                next.flip(pos);
                BOOST_CHECK(next == prev);
        }
};

struct count_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) noexcept
        {
                // [bitset.members]/34
                auto expected = 0;
                for (auto i = 0; i < N; ++i) {
                        expected += is.test(i);
                }
                BOOST_CHECK_EQUAL(is.count(), expected);

                BOOST_CHECK_EQUAL(is.count(), count(is));
        }
};

struct for_each_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
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
};

struct reverse_for_each_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        auto expected = 0;
                        reverse_for_each(is, [&](auto) { ++expected; });
                        BOOST_CHECK_EQUAL(is.count(), expected);

                        BOOST_CHECK_EQUAL(is.count(), std::distance(is.rbegin(), is.rend()));
                        BOOST_CHECK_EQUAL(is.count(), std::distance(is.crbegin(), is.crend()));
                        BOOST_CHECK_EQUAL(is.count(), std::distance(rbegin(is), rend(is)));
                        BOOST_CHECK_EQUAL(is.count(), std::distance(crbegin(is), crend(is)));
                }
        }
};

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
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/36
                auto expected = true;
                for (auto i = 0; i < N; ++i) {
                        expected &= lhs.test(i) == rhs.test(i);
                }
                BOOST_CHECK_EQUAL(lhs == rhs, expected);
        }
};

struct op_not_equal_to
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                // [bitset.members]/37
                BOOST_CHECK_EQUAL(lhs != rhs, !(lhs == rhs));
        }
};

struct op_less
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto expected = false;
                for (auto i = N - 1; i >= 0; --i) {
                        if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
                        if (!rhs.test(i) && lhs.test(i)) {                  break; }
                }
                BOOST_CHECK_EQUAL(lhs < rhs, expected);
        }
};

struct op_greater
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs > rhs, rhs < lhs);
        }
};

struct op_greater_equal
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
        }
};

struct op_less_equal
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs <= rhs, !(rhs < lhs));
        }
};

struct is_subset_of_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto expected = true;
                for (auto i = 0; i < N; ++i) {
                        expected &= !lhs.test(i) || rhs.test(i);
                }
                BOOST_CHECK_EQUAL(is_subset_of(lhs, rhs), expected);
        }
};

struct is_superset_of_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(lhs, rhs), is_subset_of(rhs, lhs));
        }
};

struct is_proper_subset_of_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(lhs, rhs), is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs));
        }
};

struct is_proper_superset_of_
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(lhs, rhs), is_proper_subset_of(rhs, lhs));
        }
};

struct test
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/38
                BOOST_CHECK_THROW(is.test(N), std::out_of_range);
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const noexcept
        {
                // [bitset.members]/39
                auto value = is; value.reset(); value.insert(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        if (i == pos) {
                                BOOST_CHECK( value.test(i));
                        } else {
                                BOOST_CHECK(!value.test(i));
                        }
                }
        }
};

struct all
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/40
                BOOST_CHECK_EQUAL(is.all(), is.count() == is.size());

                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(is.all(), is.full());
                }
        }
};

struct any
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/41
                BOOST_CHECK_EQUAL(is.any(), is.count() != 0);

                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(is.any(), !is.empty());
                }
        }
};

struct none
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                // [bitset.members]/42
                BOOST_CHECK_EQUAL(is.none(), is.count() == 0);

                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(is.none(), is.empty());
                        BOOST_CHECK_EQUAL(is.none(), empty(is));
                }
        }
};

struct op_shift_left
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is, int n) const
        {
                // [bitset.members]/43
                auto expected = is;
                expected <<= n;
                BOOST_CHECK(is << n == expected);
        }
};

struct op_shift_right
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is, int n) const
        {
                // [bitset.members]/44
                auto expected = is;
                expected >>= n;
                BOOST_CHECK(is >> n == expected);
        }
};

// 20.9.4 bitset operators [bitset.operators]

struct op_bitand
{
        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a & a) == a);                              // idempotent
        }

        template<template<auto> class IntSet, auto N>
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

        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b, IntSet<N> const& c) const noexcept
        {
                BOOST_CHECK(((a & b) & c) == (a & (b & c)));            // associative
                BOOST_CHECK((a & (b | c)) == ((a & b) | (a & c)));      // distributive
        }
};

struct op_bitor
{
        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a | a) == a);                              // idempotent
        }

        template<template<auto> class IntSet, auto N>
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

        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b, IntSet<N> const& c) const noexcept
        {
                BOOST_CHECK(((a | b) | c) == (a | (b | c)));            // associative
                BOOST_CHECK((a | (b & c)) == ((a | b) & (a | c)));      // distributive
        }
};

struct op_xor
{
        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a ^ a).none());                            // nilpotent
        }

        template<template<auto> class IntSet, auto N>
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

        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a, IntSet<N> const& b, IntSet<N> const& c) const noexcept
        {
                BOOST_CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));            // associative
                BOOST_CHECK((a & (b ^ c)) == ((a & b) ^ (a & c)));      // distributive
        }
};

struct op_minus
{
        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                BOOST_CHECK((a - a).none());                            // nilpotent
        }

        template<template<auto> class IntSet, auto N>
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
