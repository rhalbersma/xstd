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
constexpr auto op_and_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
constexpr auto op_or_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
constexpr auto op_xor_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
constexpr auto op_minus_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
auto op_shift_left_assign(int_set<N> const& b, int const pos)
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
auto op_shift_right_assign(int_set<N> const& b, int const pos)
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
auto set_all(int_set<N> const& b) noexcept
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
constexpr auto set_one(int_set<N> const& b, int const pos)
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
auto reset_all(int_set<N> const& b) noexcept
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
constexpr auto reset_one(int_set<N> const& b, int const pos)
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
constexpr auto op_complement(int_set<N> const& b) noexcept
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
auto count(int_set<N> const& b) noexcept
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

// [bitset.members]/36
template<int N>
auto op_equal_to(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
auto op_not_equal_to(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto const expected = !(lhs == rhs);

        // act
        auto const value = lhs != rhs;

        // assert
        return value == expected;
}

template<int N>
auto op_less(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
auto op_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = true;
        for (auto i = 0; i < N; ++i) {
                expected &= !lhs.test(i) || rhs.test(i);
        }

        // act
        auto const value = subset_of(lhs, rhs);

        // assert
        return value == expected;
}

template<int N>
auto op_proper_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto expected = false;
        for (auto i = 0; i < N; ++i) {
                if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
        }
        expected &= subset_of(lhs, rhs);

        // act
        auto const value = proper_subset_of(lhs, rhs);

        // assert
        return value == expected;
}

// [bitset.members]/40
template<int N>
auto all(int_set<N> const& b) noexcept
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
auto any(int_set<N> const& b) noexcept
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
auto none(int_set<N> const& b) noexcept
{
        // arrange
        auto const expected = b.size() == 0;

        // act
        auto const value = b.empty();

        // assert
        return value == expected;
}

// [bitset.members]/43
template<int N>
auto op_shift_left(int_set<N> const& b, int n)
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
auto op_shift_right(int_set<N> const& b, int n)
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
constexpr auto op_and(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
constexpr auto op_or(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
constexpr auto op_xor(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
constexpr auto op_minus(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
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
