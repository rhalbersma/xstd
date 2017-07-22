#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bitset.hpp>
#include <xstd/int_set.hpp>             // int_set
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance
#include <memory>                       // addressof
#include <stdexcept>                    // out_of_range
#include <type_traits>                  // is_same_v

namespace xstd {
namespace prim {

// bitset constructors [bitset.cons]

template<class T>
struct constructor
{
        constexpr auto operator()() const noexcept
        {
                // [bitset.cons]/1
                BOOST_CHECK(T{}.none());
        }
};

// bitset members [bitset.members]

struct op_bitand_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = prev;
                auto const& ret = (next &= rhs);

                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/1
                        if (!rhs[i]) {
                                BOOST_CHECK(!next[i]);
                        } else {
                                BOOST_CHECK_EQUAL(next[i], prev[i]);
                        }
                }

                // [bitset.members]/2
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }
};

struct op_bitor_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = prev;
                auto const& ret = (next |= rhs);

                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/3
                        if (rhs[i]) {
                                BOOST_CHECK(next[i]);
                        } else {
                                BOOST_CHECK_EQUAL(next[i], prev[i]);
                        }
                }

                // [bitset.members]/4
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }
};

struct op_xor_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto const prev = lhs;
                auto next = prev;
                auto const& ret = (next ^= rhs);

                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/5
                        if (rhs[i]) {
                                BOOST_CHECK_EQUAL(next[i], !prev[i]);
                        } else {
                                BOOST_CHECK_EQUAL(next[i],  prev[i]);
                        }
                }

                // [bitset.members]/6
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }
};

struct op_minus_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        constexpr auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        auto const prev = lhs;
                        auto next = prev;
                        auto const& ret = (next -= rhs);
                        for (auto i = SizeType{0}; i < N; ++i) {
                                if (rhs[i]) {
                                        BOOST_CHECK(!next[i]);
                                } else {
                                        BOOST_CHECK_EQUAL(next[i], prev[i]);
                                }
                        }

                        BOOST_CHECK(std::addressof(ret) == std::addressof(next));
                }
        }
};

struct op_shift_left_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                auto const prev = is;
                auto next = prev;
                auto const& ret = (next <<= pos);

                for (auto I = SizeType{0}; I < N; ++I) {
                        if (I < pos) {
                                // [bitset.members]/7.1
                                BOOST_CHECK(!next[I]);
                        } else {
                                // [bitset.members]/7.2
                                BOOST_CHECK_EQUAL(next[I], prev[I - pos]);
                        }
                }

                // [bitset.members]/8
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }
};

struct op_shift_right_assign
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                auto const prev = is;
                auto next = prev;
                auto const& ret = (next >>= pos);

                for (auto I = SizeType{0}; I < N; ++I) {
                        if (pos >= N - I) {
                                // [bitset.members]/9.1
                                BOOST_CHECK(!next[I]);
                        } else {
                                // [bitset.members]/9.2
                                BOOST_CHECK_EQUAL(next[I], prev[I + pos]);
                        }
                }

                // [bitset.members]/10
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }
};

struct set
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                auto value = is;
                auto const& ret = value.set();

                // [bitset.members]/11
                BOOST_CHECK(value.all());

                // [bitset.members]/12
                BOOST_CHECK(std::addressof(ret) == std::addressof(value));

                // [bitset.members]/13
                BOOST_CHECK_THROW(value.set(N), std::out_of_range);
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                auto const prev = is;
                auto next = prev;
                auto const& ret = next.set(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/14
                        if (i == pos) {
                                BOOST_CHECK(next[i]);
                        } else {
                                BOOST_CHECK_EQUAL(next[i], prev[i]);
                        }
                }

                // [bitset.members]/15
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }
};

struct reset
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                auto value = is;
                auto const& ret = value.reset();

                // [bitset.members]/16
                BOOST_CHECK(value.none());

                // [bitset.members]/17
                BOOST_CHECK(std::addressof(ret) == std::addressof(value));

                // [bitset.members]/18
                BOOST_CHECK_THROW(value.reset(N), std::out_of_range);
        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                auto const prev = is;
                auto next = prev;
                auto const& ret = next.reset(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/19
                        if (i == pos) {
                                BOOST_CHECK(!next[i]);
                        } else {
                                BOOST_CHECK_EQUAL(next[i], prev[i]);
                        }
                }

                // [bitset.members]/20
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));
        }

        template<auto N, class InputIterator>
        auto operator()(int_set<N> const& is, InputIterator first, InputIterator last) const
        {
                auto const prev = is;
                auto next = prev; next.erase(first, last);
                while (first != last) {
                        BOOST_CHECK(!next.contains(*first++));
                }
        }

        template<class SizeType, SizeType N>
        constexpr auto operator()(int_set<N> const& is, std::initializer_list<SizeType> ilist) const
        {
                auto const prev = is;
                auto next = prev; next.erase(ilist);
                for (auto&& elem : ilist) {
                        BOOST_CHECK(!next.contains(elem));
                }
        }
};

struct op_compl
{
        template<template<auto> class IntSet, auto N>
        constexpr auto operator()(IntSet<N> const& a) const noexcept
        {
                auto expected = a; expected.flip();
                auto const& ret = ~a;

                // [bitset.members]/21
                BOOST_CHECK(std::addressof(ret) != std::addressof(expected));

                // [bitset.members]/22
                BOOST_CHECK(ret == expected);

                BOOST_CHECK(~(~a) == a);                                // involution
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
                auto const prev = is;
                auto next = is;
                auto const& ret = next.flip();

                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/23
                        BOOST_CHECK_NE(next[i], prev[i]);
                }

                // [bitset.members]/24
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));

                // [bitset.members]/25
                BOOST_CHECK_THROW(next.flip(N), std::out_of_range);

                next.flip();
                BOOST_CHECK(next == prev);                              // involution

        }

        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType const pos) const
        {
                auto const prev = is;
                auto next = is;
                auto const& ret = next.flip(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/26
                        if (i == pos) {
                                BOOST_CHECK_NE   (next[i], prev[i]);
                        } else {
                                BOOST_CHECK_EQUAL(next[i], prev[i]);
                        }
                }

                // [bitset.members]/27
                BOOST_CHECK(std::addressof(ret) == std::addressof(next));

                next.flip(pos);
                BOOST_CHECK(next == prev);                              // involution
        }
};

struct to_ulong {};
struct to_ullong {};
struct to_string {};

struct count_
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is) noexcept
        {
                auto expected = 0;
                for (auto i = SizeType{0}; i < N; ++i) {
                        expected += is[i];
                }
                BOOST_CHECK_EQUAL(is.count(), expected);                // [bitset.members]/34
        }
};

struct for_each_
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                auto expected = SizeType{0};
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
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(is.size(), N);                        // [bitset.members]/35

                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(is.size(), is.max_size());
                }
        }
};

struct op_equal_to
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto expected = true;
                for (auto i = SizeType{0}; i < N; ++i) {
                        expected &= lhs[i] == rhs[i];
                }
                // [bitset.members]/36
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
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        auto expected = false;
                        for (auto i = N - 1; i >= 0; --i) {
                                if (!lhs[i] && rhs[i]) { expected = true; break; }
                                if (!rhs[i] && lhs[i]) {                  break; }
                        }
                        BOOST_CHECK_EQUAL(lhs < rhs, expected);
                }
        }
};

struct op_greater
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(lhs > rhs, rhs < lhs);
                }
        }
};

struct op_greater_equal
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
                }
        }
};

struct op_less_equal
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(lhs <= rhs, !(rhs < lhs));
                }
        }
};

struct is_subset_of_
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                auto expected = true;
                for (auto i = SizeType{0}; i < N; ++i) {
                        expected &= !lhs[i] || rhs[i];
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
                auto value = is; value.reset(); value.set(pos);
                for (auto i = SizeType{0}; i < N; ++i) {
                        // [bitset.members]/39
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
                BOOST_CHECK_EQUAL(is.any(), is.count() != 0);           // [bitset.members]/41

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
                BOOST_CHECK_EQUAL(is.none(), is.count() == 0);          // [bitset.members]/42

                if constexpr (std::is_same_v<IntSet<N>, int_set<N>>) {
                        BOOST_CHECK_EQUAL(is.none(), is.empty());
                        BOOST_CHECK_EQUAL(is.none(), empty(is));
                }
        }
};

struct op_shift_left
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType n) const
        {
                auto expected = is; expected <<= n;
                BOOST_CHECK(is << n == expected);                       // [bitset.members]/43
        }
};

struct op_shift_right
{
        template<class SizeType, template<SizeType> class IntSet, SizeType N>
        auto operator()(IntSet<N> const& is, SizeType n) const
        {
                auto expected = is; expected >>= n;
                BOOST_CHECK(is >> n == expected);                       // [bitset.members]/44
        }
};

struct op_at {};

// bitset hash support [bitset.hash]

struct hash_ {};

// bitset operators [bitset.operators]

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
                auto expected = a; expected &= b;
                BOOST_CHECK((a & b) == expected);                       // [bitset.operators]/1
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
                auto expected = a; expected |= b;
                BOOST_CHECK((a | b) == expected);                       // [bitset.operators]/2
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
                auto expected = a; expected ^= b;
                BOOST_CHECK((a ^ b) == expected);                       // [bitset.operators]/3
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
                        auto expected = a; expected -= b;
                        BOOST_CHECK(a - b == expected);
                }

                BOOST_CHECK(a - b == (a & ~b));
                BOOST_CHECK(a - b == (a | b) - b);
                BOOST_CHECK(a - b == a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(disjoint(a - b, b));
        }
};

struct op_istream {};
struct op_ostream {};

}       // namespace prim
}       // namespace xstd
