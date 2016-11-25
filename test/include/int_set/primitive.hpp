#pragma once
#include <xstd/int_set.hpp>      // int_set
#include <iostream>

namespace xstd {

// constructors

template<int N>
constexpr auto constructor_default() noexcept
{
        // arrange & act [bitset.cons]/1
        auto const value = int_set<N>{};

        // assert
        return value.none();
}

// comparators

template<int N>
auto op_equal_to(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange [bitset.members]/39
        auto expected = true;
        for (auto i = 0; i < N; ++i)
                expected &= lhs.test(i) == rhs.test(i);

        // act
        auto const value = lhs == rhs;

        // assert
        return value == expected;
}

template<int N>
auto op_not_equal_to(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange [bitset.members]/40
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
        for (auto i = 0; i < N; ++i)
                expected &= !lhs.test(i) || rhs.test(i);

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
        for (auto i = 0; i < N; ++i)
                if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
        expected &= subset_of(lhs, rhs);

        // act
        auto const value = proper_subset_of(lhs, rhs);

        // assert
        return value == expected;
}

// modifiers

template<int N>
constexpr auto set_one(int_set<N> const& b, int pos) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.set(pos);

        // assert [bitset.members]/15
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (i == pos)
                        check &= next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
constexpr auto reset_one(int_set<N> const& b, int pos) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.reset(pos);

        // assert [bitset.members]/21
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (i == pos)
                        check &= !next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
constexpr auto flip_one(int_set<N> const& b, int pos) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.flip(pos);

        // assert [bitset.members]/29
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (i == pos)
                        check &= next.test(i) != prev.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
auto set_all(int_set<N> const& b) noexcept
{
        // arrange
        auto value = b;

        // act
        value.set();

        // assert [bitset.members]/11
        return value.all();
}

template<int N>
auto reset_all(int_set<N> const& b) noexcept
{
        // arrange
        auto value = b;

        // act
        value.reset();

        // assert [bitset.members]/17
        return value.none();
}

template<int N>
constexpr auto flip_all(int_set<N> const& b) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.flip();

        // assert [bitset.members]/25
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= next.test(i) != prev.test(i);
        return check;
}

template<int N>
constexpr auto op_and_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next &= rhs;

        // assert [bitset.members]/1
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (!rhs.test(i))
                        check &= !next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
constexpr auto op_or_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next |= rhs;

        // assert [bitset.members]/3
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (rhs.test(i))
                        check &= next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
constexpr auto op_xor_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next ^= rhs;

        // assert [bitset.members]/5
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (rhs.test(i))
                        check &= next.test(i) == !prev.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
constexpr auto op_minus_assign(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next -= rhs;

        // assert
        auto check = true;
        for (auto i = 0; i < N; ++i)
                if (rhs.test(i))
                        check &= !next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<int N>
auto op_shift_left_assign(int_set<N> const& b, int pos)
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next <<= pos;

        // assert [bitset.members]/7
        auto check = true;
        for (auto I = 0; I < N; ++I)
                if (I < pos)
                        check &= !next.test(I);
                else
                        check &= next.test(I) == prev.test(I - pos);
        return check;
}

template<int N>
auto op_shift_right_assign(int_set<N> const& b, int pos)
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next >>= pos;

        // assert [bitset.members]/9
        auto check = true;
        for (auto I = 0; I < N; ++I)
                if (pos >= N - I)
                        check &= !next.test(I);
                else
                        check &= next.test(I) == prev.test(I + pos);
        return check;
}

// observers

template<int N>
auto all(int_set<N> const& b) noexcept
{
        // arrange [bitset.members]/44
        auto const expected = b.count() == b.size();

        // act
        auto const value = b.all();

        // assert
        return value == expected;
}

template<int N>
auto any(int_set<N> const& b) noexcept
{
        // arrange [bitset.members]/45
        auto const expected = b.count() != 0;

        // act
        auto const value = b.any();

        // assert
        return value == expected;
}

template<int N>
auto none(int_set<N> const& b) noexcept
{
        // arrange [bitset.members]/46
        auto const expected = b.count() == 0;

        // act
        auto const value = b.none();

        // assert
        return value == expected;
}

template<int N>
auto count(int_set<N> const& b) noexcept
{
        // arrange [bitset.members]/37
        auto expected = 0;
        for (auto i = 0; i < N; ++i)
                expected += b.test(i);

        // act
        auto const value = b.count();

        // assert
        return value == expected;
}

// operators

template<int N>
constexpr auto op_complement(int_set<N> const& b) noexcept
{
        // arrange [bitset.members]/24
        auto expected = b;
        expected.flip();

        // act
        auto const value = ~b;

        // assert
        return value == expected;
}

template<int N>
constexpr auto op_and(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange [bitset.operators]/1
        auto expected = lhs;
        expected &= rhs;

        // act
        auto const value = lhs & rhs;

        // assert
        return value == expected;
}

template<int N>
constexpr auto op_or(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange [bitset.operators]/2
        auto expected = lhs;
        expected |= rhs;

        // act
        auto const value = lhs | rhs;

        // assert
        return value == expected;
}

template<int N>
constexpr auto op_xor(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        // arrange [bitset.operators]/3
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

template<int N>
auto op_shift_left(int_set<N> const& b, int n)
{
        // arrange [bitset.members]/47
        auto expected = b;
        expected <<= n;

        // act
        auto const value = b << n;

        // assert
        return value == expected;
}

template<int N>
auto op_shift_right(int_set<N> const& b, int n)
{
        // arrange [bitset.members]/48
        auto expected = b;
        expected >>= n;

        // act
        auto const value = b >> n;

        // assert
        return value == expected;
}

}       // namespace xstd
