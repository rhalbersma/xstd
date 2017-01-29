#pragma once
#include <xstd/int_set.hpp>     // int_set
#include <iostream>

namespace xstd {

// 20.9.1 bitset constructors [bitset.cons]

// [bitset.cons]/1
template<int N>
constexpr auto cons_default() noexcept
{
        // arrange & act
        auto const value = int_set<N>{};

        // assert
        return value.empty();
}

// 20.9.2 bitset members [bitset.members]

// [bitset.members]/1
template<int N>
constexpr auto bitand_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const prev = lhs;
        auto next = lhs;

        // act
        next &= rhs;

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (!rhs.test(i)) {
                        check &= !next.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

// [bitset.members]/3
template<int N>
constexpr auto bitor_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const prev = lhs;
        auto next = lhs;

        // act
        next |= rhs;

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (rhs.test(i)) {
                        check &= next.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

// [bitset.members]/5
template<int N>
constexpr auto xor_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const prev = lhs;
        auto next = lhs;

        // act
        next ^= rhs;

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (rhs.test(i)) {
                        check &= next.test(i) == !prev.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

template<int N>
constexpr auto minus_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const prev = lhs;
        auto next = lhs;

        // act
        next -= rhs;

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (rhs.test(i)) {
                        check &= !next.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

// [bitset.members]/7
template<int N>
auto shift_left_assign(int_set<N> const& b, int const pos)
{
        // arrange
        auto const prev = b;
        auto next = b;

        // act
        next <<= pos;

        // assert
        auto check = true;
        for (auto I = 0; I < N; ++I) {
                if (I < pos) {
                        check &= !next.test(I);
                } else {
                        check &= next.test(I) == prev.test(I - pos);
                }
        }
        return check;
}

// [bitset.members]/9
template<int N>
auto shift_right_assign(int_set<N> const& b, int const pos)
{
        // arrange
        auto const prev = b;
        auto next = b;

        // act
        next >>= pos;

        // assert
        auto check = true;
        for (auto I = 0; I < N; ++I) {
                if (pos >= N - I) {
                        check &= !next.test(I);
                } else {
                        check &= next.test(I) == prev.test(I + pos);
                }
        }
        return check;
}

// [bitset.members]/11
template<int N>
auto fill(int_set<N> const& b) noexcept
{
        // arrange
        auto value = b;

        // act
        value.fill();

        // assert
        return value.full();
}

// [bitset.members]/14
template<int N>
constexpr auto insert(int_set<N> const& b, int const pos)
{
        // arrange
        auto const prev = b;
        auto next = b;

        // act
        next.insert(pos);

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        check &= next.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

// [bitset.members]/16
template<int N>
auto clear(int_set<N> const& b) noexcept
{
        // arrange
        auto value = b;

        // act
        value.clear();

        // assert
        return value.empty();
}

// [bitset.members]/19
template<int N>
constexpr auto erase(int_set<N> const& b, int const pos)
{
        // arrange
        auto const prev = b;
        auto next = b;

        // act
        next.erase(pos);

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        check &= !next.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

// [bitset.members]/21
template<int N>
constexpr auto compl_(int_set<N> const& b) noexcept
{
        // arrange
        auto expected = b;
        expected.flip();

        // act
        auto const value = ~b;

        // assert
        return value == expected;
}

// [bitset.members]/23
template<int N>
constexpr auto flip_all(int_set<N> const& b) noexcept
{
        // arrange
        auto const prev = b;
        auto next = b;

        // act
        next.flip();

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= next.test(i) != prev.test(i);
        }
        return check;
}

// [bitset.members]/26
template<int N>
constexpr auto flip_one(int_set<N> const& b, int const pos)
{
        // arrange
        auto const prev = b;
        auto next = b;

        // act
        next.flip(pos);

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        check &= next.test(i) != prev.test(i);
                } else {
                        check &= next.test(i) == prev.test(i);
                }
        }
        return check;
}

// [bitset.members]/34
template<int N>
auto size_(int_set<N> const& b) noexcept
{
        // arrange
        auto expected = 0;
        for (auto i = 0; i < N; ++i) {
                expected += b.test(i);
        }

        // act
        auto const value = b.size();

        // assert
        return value == expected;
}

template<int N>
auto non_member_size(int_set<N> const& b) noexcept
{
        // arrange
        auto expected = b.size();

        // act
        auto const value = size(b);

        // assert
        return value == expected;
}

// [bitset.members]/35
template<int N>
auto max_size(int_set<N> const& b) noexcept
{
        // arrange
        auto const expected = N;

        // act
        auto const value = b.max_size();

        // assert
        return value == expected;
}

// [bitset.members]/36
template<int N>
auto equal_to(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = true;
        for (auto i = 0; i < N; ++i) {
                expected &= lhs.test(i) == rhs.test(i);
        }

        // act
        auto const value = lhs == rhs;

        // assert
        return value == expected;
}

// [bitset.members]/37
template<int N>
auto not_equal_to(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const expected = !(lhs == rhs);

        // act
        auto const value = lhs != rhs;

        // assert
        return value == expected;
}

template<int N>
auto less(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = false;
        for (auto i = N - 1; i >= 0; --i) {
                if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
                if (!rhs.test(i) && lhs.test(i)) {                  break; }
        }

        // act
        auto const value = lhs < rhs;

        // assert
        return value == expected;
}

template<int N>
auto greater(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = rhs < lhs;

        // act
        auto const value = lhs > rhs;

        // assert
        return value == expected;
}

template<int N>
auto greater_equal(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = !(lhs < rhs);

        // act
        auto const value = lhs >= rhs;

        // assert
        return value == expected;
}

template<int N>
auto less_equal(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = !(rhs < lhs);

        // act
        auto const value = lhs <= rhs;

        // assert
        return value == expected;
}

template<int N>
auto is_subset_of_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = true;
        for (auto i = 0; i < N; ++i) {
                expected &= !lhs.test(i) || rhs.test(i);
        }

        // act
        auto const value = is_subset_of(lhs, rhs);

        // assert
        return value == expected;
}

template<int N>
auto is_superset_of_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = is_subset_of(rhs, lhs);

        // act
        auto const value = is_superset_of(lhs, rhs);

        // assert
        return value == expected;
}

template<int N>
auto is_proper_subset_of_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const expected = is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);

        // act
        auto const value = is_proper_subset_of(lhs, rhs);

        // assert
        return value == expected;
}

template<int N>
auto is_proper_superset_of_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const expected = is_proper_subset_of(rhs, lhs);

        // act
        auto const value = is_proper_superset_of(lhs, rhs);

        // assert
        return value == expected;
}

// [bitset.members]/39
template<int N>
auto test(int const pos) noexcept
{
        // arrange
        auto b = int_set<N>{};

        // act
        b.insert(pos);

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        check &= b.test(i);
                } else {
                        check &= !b.test(i);
                }
        }
        return check;
}

// [bitset.members]/40
template<int N>
auto full(int_set<N> const& b) noexcept
{
        // arrange
        auto const expected = b.size() == b.max_size();

        // act
        auto const value = b.full();

        // assert
        return value == expected;
}

// [bitset.members]/41
template<int N>
auto not_empty(int_set<N> const& b) noexcept
{
        // arrange
        auto const expected = b.size() != 0;

        // act
        auto const value = !b.empty();

        // assert
        return value == expected;
}

// [bitset.members]/42
template<int N>
auto empty_(int_set<N> const& b) noexcept
{
        // arrange
        auto const expected = b.size() == 0;

        // act
        auto const value = b.empty();

        // assert
        return value == expected;
}

template<int N>
auto non_member_empty(int_set<N> const& b) noexcept
{
        // arrange
        auto const expected = b.empty();

        // act
        auto const value = empty(b);

        // assert
        return value == expected;
}

// [bitset.members]/43
template<int N>
auto shift_left(int_set<N> const& b, int n)
{
        // arrange
        auto expected = b;
        expected <<= n;

        // act
        auto const value = b << n;

        // assert
        return value == expected;
}

// [bitset.members]/44
template<int N>
auto shift_right(int_set<N> const& b, int n)
{
        // arrange
        auto expected = b;
        expected >>= n;

        // act
        auto const value = b >> n;

        // assert
        return value == expected;
}

// 20.9.4 bitset operators [bitset.operators]

// [bitset.operators]/1
template<int N>
constexpr auto bitand_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = lhs;
        expected &= rhs;

        // act
        auto const value = lhs & rhs;

        // assert
        return value == expected;
}

// [bitset.operators]/2
template<int N>
constexpr auto bitor_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = lhs;
        expected |= rhs;

        // act
        auto const value = lhs | rhs;

        // assert
        return value == expected;
}

// [bitset.operators]/3
template<int N>
constexpr auto xor_(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = lhs;
        expected ^= rhs;

        // act
        auto const value = lhs ^ rhs;

        // assert
        return value == expected;
}

template<int N>
constexpr auto minus(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = lhs;
        expected -= rhs;

        // act
        auto const value = lhs - rhs;

        // assert
        return value == expected;
}

}       // namespace xstd
