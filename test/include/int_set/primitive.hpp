#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/traits.hpp>
#include <xstd/bitset.hpp>
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <algorithm>                    // for_each
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance
#include <istream>                      // basic_istream
#include <locale>                       // ctype, use_facet
#include <memory>                       // addressof
#include <sstream>                      // basic_stringstream
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
                charT /* nil */ = charT('0'), charT /* one */ = charT('1')
        ) const
        {
                                                                                // [bitset.cons]/3-5
        }

        template <class charT>
        auto operator()(
                const charT* /* str */,
                typename std::basic_string<charT>::size_type /* n */ = std::basic_string<charT>::npos,
                charT /* nil */ = charT('0'), charT /* one */ = charT('1')
        ) const
        {
                                                                                // [bitset.cons]/6-7
        }

        template<class InputIterator>
        auto operator()(InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (std::is_constructible_v<IntSet, InputIterator, InputIterator>) {
                        auto const dst = IntSet(first, last);
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_EQUAL(dst.count(), std::distance(first, last));
                }
        }

        template<class ValueType>
        constexpr auto operator()(std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (std::is_constructible_v<IntSet, std::initializer_list<ValueType>>) {
                        auto const dst = IntSet(ilist);
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_EQUAL(dst.count(), ilist.size());
                }
        }
};

struct assign
{
        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is [[maybe_unused]], InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (tti::has_range_assign_v<IntSet, InputIterator>) {
                        auto const src = is;
                        auto dst = src; dst.assign(first, last);
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_EQUAL(dst.count(), std::distance(first, last));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_assign_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst = ilist;
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_EQUAL(dst.count(), ilist.size());
                }
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

                        for (auto&& ref : is) {
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
                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto is = cis;

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

struct front
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                if constexpr (tti::has_member_front_v<IntSet> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK(is.none() || (is.front() == *is.begin()));
                        BOOST_CHECK(is.none() || (&is.front() == is.begin()));
                }
        }
};

struct back
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                if constexpr (tti::has_member_back_v<IntSet> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK(is.none() || (is.back() == *is.rbegin()));
                        BOOST_CHECK(is.none() || (&is.back() == std::next(is.rbegin()).base()));
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
                auto const lambda [[maybe_unused]]= [&](auto const pos) { BOOST_CHECK(is[pos]); };
                if constexpr (tti::has_reverse_for_each_v<IntSet, decltype(lambda)>) {
                        reverse_for_each(is, lambda);
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

struct insert
{
        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is [[maybe_unused]], InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (tti::has_range_insert_v<IntSet, InputIterator>) {
                        auto const src = is;
                        auto dst = src; dst.insert(first, last);
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_LE(src.count(), dst.count());
                        BOOST_CHECK_LE(dst.count(), src.count() + std::distance(first, last));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_insert_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst.insert(ilist);
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_LE(src.count(), dst.count());
                        BOOST_CHECK_LE(dst.count(), src.count() + static_cast<int>(ilist.size()));
                }
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
};

struct erase
{
        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is [[maybe_unused]], InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (tti::has_range_erase_v<IntSet, InputIterator>) {
                        auto const src = is;
                        auto dst = src; dst.erase(first, last);
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(!dst.contains(elem));
                        });
                        BOOST_CHECK_LE(dst.count(), src.count());
                        BOOST_CHECK_LE(src.count(), dst.count() + std::distance(first, last));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_erase_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst.erase(ilist);
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(!dst.contains(elem));
                        }
                        BOOST_CHECK_LE(dst.count(), src.count());
                        BOOST_CHECK_LE(src.count(), dst.count() + static_cast<int>(ilist.size()));
                }
        }
};

struct swap_
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto src1 = a, src2 = b;
                using std::swap;
                swap(src1, src2);
                BOOST_CHECK(src1 == b && src2 == a);
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

struct count
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
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(is.size(), IntSet{}.size());                  // [bitset.members]/35

                if constexpr (tti::has_static_member_max_size_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.size(), is.max_size());
                }
        }
};

template<class IntSet>
struct capacity
{
        auto operator()() const noexcept
        {
                if constexpr (tti::has_static_member_capacity_v<IntSet>) {
                        using block_type = typename IntSet::block_type;
                        constexpr auto block_size = std::numeric_limits<block_type>::digits;

                        BOOST_CHECK_EQUAL(IntSet::capacity() % block_size, 0);
                        BOOST_CHECK_EQUAL(IntSet::capacity() / block_size, (IntSet::max_size() - 1 + block_size) / block_size);
                }
        }
};

struct op_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(a == a);                                            // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = true;
                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        expected &= a[i] == b[i];
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36
                BOOST_CHECK_EQUAL(a == b, b == a);                              // symmetric
        }
};

struct op_not_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!(a != a));                                         // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a != b, !(a == b));                           // [bitset.members]/37
                BOOST_CHECK_EQUAL(a != b, b != a);                              // symmetric
        }
};

struct op_less
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!(a < a));                                          // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = false;
                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        auto const r = N - 1 - i;
                        if (!a[r] && b[r]) { expected = true; break; }
                        if (!b[r] && a[r]) {                  break; }
                }
                BOOST_CHECK_EQUAL(a < b, expected);
                BOOST_CHECK_EQUAL(a < b, a.to_string() < b.to_string());
        }
};

struct op_greater
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!(a > a));                                          // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a > b, b < a);
        }
};

struct op_greater_equal
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(a >= a);                                            // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
        }
};

struct op_less_equal
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(a <= a);                                            // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a <= b, !(b < a));
        }
};

struct is_subset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(is_subset_of(a, a));                                // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = true;
                for (auto N = IntSet{}.size(), i = decltype(N){0}; i < N; ++i) {
                        expected &= !a[i] || b[i];
                }
                BOOST_CHECK_EQUAL(is_subset_of(a, b), expected);
        }
};

struct is_superset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(is_superset_of(a, a));                              // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(a, b), is_subset_of(b, a));
        }
};

struct is_proper_subset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!is_proper_subset_of(a, a));                        // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(a, b), is_subset_of(a, b) && !is_subset_of(b, a));
        }
};

struct is_proper_superset_of_
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!is_proper_superset_of(a, a));                      // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(a, b), is_proper_subset_of(b, a));
        }
};

struct intersects_
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(intersects(a, a) || a.none());
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(intersects(a, b), (a & b).any());
                BOOST_CHECK_EQUAL(intersects(a, b), intersects(b, a));
        }
};

struct disjoint_
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!disjoint(a, a) || a.none());
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(disjoint(a, b), !intersects(a, b));
                BOOST_CHECK_EQUAL(disjoint(a, b), disjoint(b, a));
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
        auto operator()(std::basic_istream<charT, traits>& /* istr */, IntSet& /* is */) const
        {
                                                                                // [bitset.operators]/4-7
        }
};

struct op_ostream
{
        template<class charT, class traits, class IntSet>
        auto operator()(std::basic_stringstream<charT, traits>& ostr, IntSet const& is) const
        {
                auto const expected = is.template to_string<charT, traits, std::allocator<charT>>(
                        std::use_facet<std::ctype<charT>>(ostr.getloc()).widen('0'),
                        std::use_facet<std::ctype<charT>>(ostr.getloc()).widen('1')
                );
                ostr << is;
                BOOST_CHECK_EQUAL(ostr.str(), expected);                        // [bitset.operators]/8
        }
};

}       // namespace test
}       // namespace xstd
