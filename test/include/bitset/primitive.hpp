#pragma once
#include <xstd/bitset.hpp>      // bitset
#include <xstd/cstddef.hpp>     // 0_z
#include <cstddef>              // size_t

namespace xstd {

// constructors

template<std::size_t N>
constexpr auto constructor_default() noexcept
{
        // arrange & act [bitset.cons]/1
        auto const value = bitset<N>{};

        // assert
        return value.none();
}

// comparators

template<std::size_t N>
constexpr auto op_equal_to(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange [bitset.members]/39
        auto expected = true;
        for (auto i = 0_z; i < N; ++i)
                expected &= lhs.test(i) == rhs.test(i);

        // act
        auto const value = lhs == rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_not_equal_to(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange [bitset.members]/40
        auto const expected = !(lhs == rhs);

        // act
        auto const value = lhs != rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_less(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto expected = false;
        for (auto i = N - 1; i < N; --i) {
                if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
                if (!rhs.test(i) && lhs.test(i)) {                  break; }
        }

        // act
        auto const value = lhs < rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto is_subset_of(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto expected = true;
        for (auto i = 0_z; i < N; ++i)
                expected &= !lhs.test(i) || rhs.test(i);

        // act
        auto const value = lhs.is_subset_of(rhs);

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto is_proper_subset_of(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto expected = false;
        for (auto i = 0_z; i < N; ++i)
                if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
        expected &= lhs.is_subset_of(rhs);

        // act
        auto const value = lhs.is_proper_subset_of(rhs);

        // assert
        return value == expected;
}

// modifiers

template<std::size_t N>
constexpr auto set_one(bitset<N> const& b, std::size_t pos) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.set(pos);

        // assert [bitset.members]/15
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (i == pos)
                        check &= next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto reset_one(bitset<N> const& b, std::size_t pos) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.reset(pos);

        // assert [bitset.members]/21
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (i == pos)
                        check &= !next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto flip_one(bitset<N> const& b, std::size_t pos) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.flip(pos);

        // assert [bitset.members]/29
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (i == pos)
                        check &= next.test(i) != prev.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto set_all(bitset<N> const& b) noexcept
{
        // arrange
        auto value = b;

        // act
        value.set();

        // assert [bitset.members]/11
        return value.all();
}

template<std::size_t N>
constexpr auto reset_all(bitset<N> const& b) noexcept
{
        // arrange
        auto value = b;

        // act
        value.reset();

        // assert [bitset.members]/17
        return value.none();
}

template<std::size_t N>
constexpr auto flip_all(bitset<N> const& b) noexcept
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next.flip();

        // assert [bitset.members]/25
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                check &= next.test(i) != prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto op_and_assign(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next &= rhs;

        // assert [bitset.members]/1
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (!rhs.test(i))
                        check &= !next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto op_or_assign(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next |= rhs;

        // assert [bitset.members]/3
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (rhs.test(i))
                        check &= next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto op_xor_assign(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next ^= rhs;

        // assert [bitset.members]/5
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (rhs.test(i))
                        check &= next.test(i) == !prev.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto op_minus_assign(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto next = lhs;
        auto const prev = lhs;

        // act
        next -= rhs;

        // assert
        auto check = true;
        for (auto i = 0_z; i < N; ++i)
                if (rhs.test(i))
                        check &= !next.test(i);
                else
                        check &= next.test(i) == prev.test(i);
        return check;
}

template<std::size_t N>
constexpr auto op_shift_left_assign(bitset<N> const& b, std::size_t pos)
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next <<= pos;

        // assert [bitset.members]/7
        auto check = true;
        for (auto I = 0_z; I < N; ++I)
                if (I < pos)
                        check &= !next.test(I);
                else
                        check &= next.test(I) == prev.test(I - pos);
        return check;
}

template<std::size_t N>
constexpr auto op_shift_right_assign(bitset<N> const& b, std::size_t pos)
{
        // arrange
        auto next = b;
        auto const prev = b;

        // act
        next >>= pos;

        // assert [bitset.members]/9
        auto check = true;
        for (auto I = 0_z; I < N; ++I)
                if (pos >= N - I)
                        check &= !next.test(I);
                else
                        check &= next.test(I) == prev.test(I + pos);
        return check;
}

// observers

template<std::size_t N>
constexpr auto all(bitset<N> const& b) noexcept
{
        // arrange [bitset.members]/44
        auto const expected = b.count() == b.size();

        // act
        auto const value = b.all();

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto any(bitset<N> const& b) noexcept
{
        // arrange [bitset.members]/45
        auto const expected = b.count() != 0;

        // act
        auto const value = b.any();

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto none(bitset<N> const& b) noexcept
{
        // arrange [bitset.members]/46
        auto const expected = b.count() == 0;

        // act
        auto const value = b.none();

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto count(bitset<N> const& b) noexcept
{
        // arrange [bitset.members]/37
        std::size_t expected = 0;
        for (auto i = 0_z; i < N; ++i)
                expected += b.test(i);

        // act
        auto const value = b.count();

        // assert
        return value == expected;
}

// operators

template<std::size_t N>
constexpr auto op_complement(bitset<N> const& b) noexcept
{
        // arrange [bitset.members]/24
        auto expected = b;
        expected.flip();

        // act
        auto const value = ~b;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_and(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange [bitset.operators]/1
        auto expected = lhs;
        expected &= rhs;

        // act
        auto const value = lhs & rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_or(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange [bitset.operators]/2
        auto expected = lhs;
        expected |= rhs;

        // act
        auto const value = lhs | rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_xor(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange [bitset.operators]/3
        auto expected = lhs;
        expected ^= rhs;

        // act
        auto const value = lhs ^ rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_minus(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        // arrange
        auto expected = lhs;
        expected -= rhs;

        // act
        auto const value = lhs - rhs;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_shift_left(bitset<N> const& b, std::size_t n)
{
        // arrange [bitset.members]/47
        auto expected = b;
        expected <<= n;

        // act
        auto const value = b << n;

        // assert
        return value == expected;
}

template<std::size_t N>
constexpr auto op_shift_right(bitset<N> const& b, std::size_t n)
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
