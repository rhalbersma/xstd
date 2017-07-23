#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/traits.hpp>
#include <xstd/bitset.hpp>
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance
#include <istream>                      // basic_istream
#include <memory>                       // addressof
#include <ostream>                      // basic_ostream
#include <stdexcept>                    // invalid_argument, out_of_range, overflow_error
#include <string>                       // basic_string

namespace xstd {
namespace test {

template<class IntSet>
struct constructor
{
        constexpr auto operator()() const noexcept
        {
                BOOST_CHECK(IntSet{}.none());                                   // [bitset.cons]/1
        }

        constexpr auto operator()(unsigned long long const /* val */) const
        {
                                                                                // [bitset.cons]/2
        }

        template <class charT, class traits, class Allocator>
        auto operator()(
                const std::basic_string<charT, traits, Allocator>& /* str */,
                typename std::basic_string<charT, traits, Allocator>::size_type /* pos */ = 0,
                typename std::basic_string<charT, traits, Allocator>::size_type /* n */ =
                std::basic_string<charT, traits, Allocator>::npos,
                charT /* zero */ = charT('0'), charT /* one */ = charT('1')
        ) const
        {
                                                                                // [bitset.cons]/3-5
        }

        template <class charT>
        auto operator()(
                const charT* /* str */,
                typename std::basic_string<charT>::size_type /* n */ = std::basic_string<charT>::npos,
                charT /* zero */ = charT('0'), charT /* one */ = charT('1')
        ) const
        {
                                                                                // [bitset.cons]/6-7
        }
};

struct const_reference
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        for (auto first = is.begin(), last = is.end(); first != last; ++first) {
                                auto const ref = *first;

                                BOOST_CHECK(&ref == first);
                                BOOST_CHECK(is[ref]);
                        }
                }
        }
};

struct const_iterator
{
        template<class IntSet>
        auto operator()(IntSet const& cis) const noexcept
        {
                auto is = cis;
                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(std::distance(cis.  begin(), cis.  end()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is.  begin(),  is.  end()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is. cbegin(),  is. cend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(cis. rbegin(), cis. rend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is. rbegin(),  is. rend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is.crbegin(),  is.crend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(  begin(cis),   end(cis)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(  begin( is),   end( is)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( cbegin( is),  cend( is)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( rbegin(cis),  rend(cis)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( rbegin( is),  rend( is)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(crbegin( is), crend( is)), cis.count());
                }
        }
};

struct for_each_
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                for_each(is, [&](auto const pos) {
                        BOOST_CHECK(is[pos]);
                });
        }
};

struct reverse_for_each_
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                if constexpr (tti::has_reverse_for_each_v<IntSet>) {
                        reverse_for_each(is, [&](auto const pos) {
                                BOOST_CHECK(is[pos]);
                        });
                }
        }
};

struct op_bitand_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto const src = lhs;
                auto dst = src;
                auto const& ret = (dst &= rhs);

                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], !rhs[i] ? false : src[i]);    // [bitset.members]/1
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/2
        }
};

struct op_bitor_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto const src = lhs;
                auto dst = src;
                auto const& ret = (dst |= rhs);

                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? true : src[i]);      // [bitset.members]/3
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/4
        }
};

struct op_xor_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto const src = lhs;
                auto dst = src;
                auto const& ret = (dst ^= rhs);

                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? !src[i] : src[i]);   // [bitset.members]/5
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/6
        }
};

struct op_minus_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                if constexpr (tti::has_member_op_minus_assign_v<IntSet>) {
                        auto const src = lhs;
                        auto dst = src;
                        auto const& ret = (dst -= rhs);

                        for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                                BOOST_CHECK_EQUAL(dst[i], rhs[i] ? false : src[i]);
                        }
                        BOOST_CHECK(std::addressof(ret) == std::addressof(dst));
                }
        }
};

struct op_shift_left_assign
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = (dst <<= pos);

                for (auto N = is.size(), I = decltype(N){0}; I < N; ++I) {
                        if (I < pos) {
                                BOOST_CHECK(!dst[I]);                           // [bitset.members]/7.1
                        } else {
                                BOOST_CHECK_EQUAL(dst[I], src[I - pos]);        // [bitset.members]/7.2
                        }
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/8
        }
};

struct op_shift_right_assign
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = (dst >>= pos);

                for (auto N = is.size(), I = decltype(N){0}; I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!dst[I]);                           // [bitset.members]/9.1
                        } else {
                                BOOST_CHECK_EQUAL(dst[I], src[I + pos]);        // [bitset.members]/9.2
                        }
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/10
        }
};

struct set
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                auto const& ret = value.set();

                BOOST_CHECK(value.all());                                       // [bitset.members]/11
                BOOST_CHECK(std::addressof(ret) == std::addressof(value));      // [bitset.members]/12
                BOOST_CHECK_THROW(value.set(is.size()), std::out_of_range);     // [bitset.members]/13
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = dst.set(pos);

                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], i == pos ? true : src[i]);    // [bitset.members]/14
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/15
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos, bool const val) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = dst.set(pos, val);

                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], i == pos ? val : src[i]);     // [bitset.members]/14
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/15
        }
};

struct reset
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                auto const& ret = value.reset();

                BOOST_CHECK(value.none());                                      // [bitset.members]/16
                BOOST_CHECK(std::addressof(ret) == std::addressof(value));      // [bitset.members]/17
                BOOST_CHECK_THROW(value.reset(is.size()), std::out_of_range);   // [bitset.members]/18
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = dst.reset(pos);

                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], i == pos ? false : src[i]);   // [bitset.members]/19
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/20
        }

        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is, InputIterator first, InputIterator last) const
        {
                auto const src = is;
                auto dst = src; dst.erase(first, last);
                while (first != last) {
                        BOOST_CHECK(!dst.contains(*first++));
                }
        }

        template<class IntSet, class SizeType>
        constexpr auto operator()(IntSet const& is, std::initializer_list<SizeType> ilist) const
        {
                auto const src = is;
                auto dst = src; dst.erase(ilist);
                for (auto&& elem : ilist) {
                        BOOST_CHECK(!dst.contains(elem));
                }
        }
};

struct op_compl
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                auto expected = a; expected.flip();
                auto const& ret = ~a;

                BOOST_CHECK(std::addressof(ret) != std::addressof(expected));   // [bitset.members]/21
                BOOST_CHECK(ret == expected);                                   // [bitset.members]/22
                BOOST_CHECK(~ret == a);                                         // involution
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK(~(a | b) == (~a & ~b));                             // De Morgan's Laws
                BOOST_CHECK(~(a & b) == (~a | ~b));                             // De Morgan's Laws
        }
};

struct flip
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto const src = is;
                auto dst = is;
                auto const& ret = dst.flip();

                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_NE(dst[i], src[i]);                         // [bitset.members]/23
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/24
                BOOST_CHECK_THROW(dst.flip(is.size()), std::out_of_range);      // [bitset.members]/25

                dst.flip();
                BOOST_CHECK(dst == src);                                        // involution
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = is;
                auto const& ret = dst.flip(pos);
                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], i == pos? !src[i] : src[i]);  // [bitset.members]/26
                }
                BOOST_CHECK(std::addressof(ret) == std::addressof(dst));        // [bitset.members]/27

                dst.flip(pos);
                BOOST_CHECK(dst == src);                                        // involution
        }
};

struct to_ulong
{
        template<class IntSet>
        auto operator()(IntSet const& /* is */) const noexcept
        {
                                                                                // [bitset.members]/28-29
        }
};

struct to_ullong
{
        template<class IntSet>
        auto operator()(IntSet const& /* is */) const noexcept
        {
                                                                                // [bitset.members]/30-31
        }
};

struct to_string
{
        template<class IntSet>
        auto operator()(IntSet const& /* is */) const noexcept
        {
                                                                                // [bitset.members]/32-33
        }
};

struct count_
{
        template<class IntSet>
        auto operator()(IntSet const& is) noexcept
        {
                auto expected = decltype(is.count()){0};
                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        expected += is[i];
                }
                BOOST_CHECK_EQUAL(is.count(), expected);                        // [bitset.members]/34
        }
};

struct size
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(is.size(), N);                                // [bitset.members]/35

                if constexpr (tti::has_static_member_max_size_v<IntSet<N>>) {
                        BOOST_CHECK_EQUAL(is.size(), is.max_size());
                }
        }
};

struct op_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto expected = true;
                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        expected &= lhs[i] == rhs[i];
                }
                BOOST_CHECK_EQUAL(lhs == rhs, expected);                        // [bitset.members]/36
        }
};

struct op_not_equal_to
{
        template<template<auto> class IntSet, auto N>
        auto operator()(IntSet<N> const& lhs, IntSet<N> const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs != rhs, !(lhs == rhs));                   // [bitset.members]/37
        }
};

struct op_less
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                if constexpr (tti::has_op_less_v<IntSet>) {
                        auto expected = false;
                        for (auto N = IntSet{}.size(), mI = decltype(N){-1}, i = N - 1; i > mI; --i) {
                                if (!lhs[i] && rhs[i]) { expected = true; break; }
                                if (!rhs[i] && lhs[i]) {                  break; }
                        }
                        BOOST_CHECK_EQUAL(lhs < rhs, expected);
                }
        }
};

struct op_greater
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                if constexpr (tti::has_op_less_v<IntSet>) {
                        BOOST_CHECK_EQUAL(lhs > rhs, rhs < lhs);
                }
        }
};

struct op_greater_equal
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                if constexpr (tti::has_op_less_v<IntSet>) {
                        BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
                }
        }
};

struct op_less_equal
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                if constexpr (tti::has_op_less_v<IntSet>) {
                        BOOST_CHECK_EQUAL(lhs <= rhs, !(rhs < lhs));
                }
        }
};

struct is_subset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto expected = true;
                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        expected &= !lhs[i] || rhs[i];
                }
                BOOST_CHECK_EQUAL(is_subset_of(lhs, rhs), expected);
        }
};

struct is_superset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(lhs, rhs), is_subset_of(rhs, lhs));
        }
};

struct is_proper_subset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(lhs, rhs), is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs));
        }
};

struct is_proper_superset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(lhs, rhs), is_proper_subset_of(rhs, lhs));
        }
};

struct test_
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_THROW(is.test(is.size()), std::out_of_range);       // [bitset.members]/38
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const noexcept
        {
                auto value = is; value.reset(); value.set(pos);
                for (auto N = is.size(), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(i == pos, value.test(i));             // [bitset.members]/39
                }
        }
};

struct all
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(is.all(), is.count() == is.size());           // [bitset.members]/40

                if constexpr (tti::has_member_full_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.all(), is.full());
                }
        }
};

struct any
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(is.any(), is.count() != 0);                   // [bitset.members]/41

                if constexpr (tti::has_member_empty_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.any(), !is.empty());
                }
        }
};

struct none
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(is.none(), is.count() == 0);                  // [bitset.members]/42

                if constexpr (tti::has_member_empty_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.none(), is.empty());
                        BOOST_CHECK_EQUAL(is.none(), empty(is));
                }
        }
};

struct op_shift_left
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                auto expected = is; expected <<= n;
                BOOST_CHECK(is << n == expected);                               // [bitset.members]/43
        }
};

struct op_shift_right
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                auto expected = is; expected >>= n;
                BOOST_CHECK(is >> n == expected);                               // [bitset.members]/44
        }
};

struct op_at
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& /* is */, SizeType const /* pos */) const
        {
                                                                                // [bitset.members]/45-47
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& /* is */, SizeType const /* pos */, bool const /* value */) const
        {
                                                                                // [bitset.members]/48-51
        }
};

struct hash_
{
                                                                                // [bitset.hash]/1
};

struct op_bitand
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a & a) == a);                                      // idempotent
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected &= b;
                BOOST_CHECK((a & b) == expected);                               // [bitset.operators]/1
                BOOST_CHECK((a & b) == (b & a));                                // commutative
                BOOST_CHECK(is_subset_of(a & b, a));
                BOOST_CHECK(is_subset_of(a & b, b));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a & b) & c) == (a & (b & c)));                    // associative
                BOOST_CHECK((a & (b | c)) == ((a & b) | (a & c)));              // distributive
        }
};

struct op_bitor
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a | a) == a);                                      // idempotent
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected |= b;
                BOOST_CHECK((a | b) == expected);                               // [bitset.operators]/2
                BOOST_CHECK((a | b) == (b | a));                                // commutative
                BOOST_CHECK(is_subset_of(a, a | b));
                BOOST_CHECK(is_subset_of(b, a | b));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a | b) | c) == (a | (b | c)));                    // associative
                BOOST_CHECK((a | (b & c)) == ((a | b) & (a | c)));              // distributive
        }
};

struct op_xor
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a ^ a).none());                                    // nilpotent
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected ^= b;
                BOOST_CHECK((a ^ b) == expected);                               // [bitset.operators]/3
                BOOST_CHECK((a ^ b) == (b ^ a));                                // commutative
                BOOST_CHECK((a ^ b) == ((a - b) | (b - a)));
                BOOST_CHECK((a ^ b) == (a | b) - (a & b));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));                    // associative
                BOOST_CHECK((a & (b ^ c)) == ((a & b) ^ (a & c)));              // distributive
        }
};

struct op_minus
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a - a).none());                                    // nilpotent
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                if constexpr (tti::has_member_op_minus_assign_v<IntSet>) {
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

struct op_istream
{
        template<class charT, class traits, class IntSet>
        auto operator()(std::basic_istream<charT, traits>& /* is */, IntSet& /* x */) const
        {
                                                                                // [bitset.operators]/4-7
        }
};

struct op_ostream
{
        template<class charT, class traits, class IntSet>
        auto operator()(std::basic_ostream<charT, traits>& /* os */, IntSet const& /* x */) const
        {
                                                                                // [bitset.operators]/8
        }
};

}       // namespace test
}       // namespace xstd
