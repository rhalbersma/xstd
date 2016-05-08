#pragma once
#include <bitset/primitive.hpp>
#include <xstd/bitset.hpp>              // bitset
#include <xstd/cstddef.hpp>             // 0_zu

namespace xstd {

// comparators

template<std::size_t N>
constexpr auto op_equal_to() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_equal_to(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_not_equal_to() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_not_equal_to(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_less() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_less(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto is_subset_of() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= is_subset_of(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto is_proper_subset_of() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= is_proper_subset_of(bitset<N>{i}, bitset<N>{j});
        return check;
}

// modifiers

template<std::size_t N>
constexpr auto set_one() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= set_one(bitset<N>{i}, j);
        return check;
}

template<std::size_t N>
constexpr auto reset_one() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= reset_one(bitset<N>{i}, j);
        return check;
}

template<std::size_t N>
constexpr auto flip_one() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= flip_one(bitset<N>{i}, j);
        return check;
}

template<std::size_t N>
constexpr auto set_all() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= set_all(~(~bitset<N>{} << i));
        check &= set_all(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto reset_all() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= reset_all(~(~bitset<N>{} << i));
        check &= reset_all(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto flip_all() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= flip_all(~(~bitset<N>{} << i));
        check &= flip_all(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto op_and_assign() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_and_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_or_assign() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_or_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_xor_assign() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_xor_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_minus_assign() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_minus_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_shift_left_assign() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_shift_left_assign(bitset<N>{i}, j);
        return check;
}

template<std::size_t N>
constexpr auto op_shift_right_assign() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_shift_right_assign(bitset<N>{i}, j);
        return check;
}

// observers

template<std::size_t N>
constexpr auto all() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= all(~(~bitset<N>{} << i));
        check &= all(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto any() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= any(~(~bitset<N>{} << i));
        check &= any(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto none() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= none(~(~bitset<N>{} << i));
        check &= none(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto count() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= count(~(~bitset<N>{} << i));
        check &= count(~bitset<N>{});
        return check;
}

// operators

template<std::size_t N>
constexpr auto op_complement() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                check &= op_complement(~(~bitset<N>{} << i));
        check &= op_complement(~bitset<N>{});
        return check;
}

template<std::size_t N>
constexpr auto op_and() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_and(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_or() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_or(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_xor() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_xor(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_minus() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_minus(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<std::size_t N>
constexpr auto op_shift_left() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_shift_left(bitset<N>{i}, j);
        return check;
}

template<std::size_t N>
constexpr auto op_shift_right() noexcept
{
        auto check = true;
        for (auto i = 0_zu; i < N; ++i)
                for (auto j = 0_zu; j < N; ++j)
                        check &= op_shift_right(bitset<N>{i}, j);
        return check;
}

}       // namespace xstd
