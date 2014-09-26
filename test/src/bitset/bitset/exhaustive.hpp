#pragma once
#include <bitset/bitset/primitive.hpp>
#include <xstd/bitset.hpp>

namespace xstd {

// comparators

template<int N>
constexpr auto op_equal_to() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_equal_to(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_not_equal_to() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_not_equal_to(bitset<N>{i}, bitset<N>{j});
        return check;
}

// modifiers

template<int N>
constexpr auto set_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= set_one(bitset<N>{i}, j);
        return check;
}

template<int N>
constexpr auto reset_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= reset_one(bitset<N>{i}, j);
        return check;
}

template<int N>
constexpr auto flip_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= flip_one(bitset<N>{i}, j);
        return check;
}

template<int N>
constexpr auto set_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= set_all(~(~bitset<N>{} << i));
        check &= set_all(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto reset_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= reset_all(~(~bitset<N>{} << i));
        check &= reset_all(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto flip_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= flip_all(~(~bitset<N>{} << i));
        check &= flip_all(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto op_and_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_and_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_or_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_or_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_xor_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_xor_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_minus_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_minus_assign(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_shift_left_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_left_assign(bitset<N>{i}, j);
        return check;
}

template<int N>
constexpr auto op_shift_right_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_right_assign(bitset<N>{i}, j);
        return check;
}

// observers

template<int N>
constexpr auto all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= all(~(~bitset<N>{} << i));
        check &= all(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto any() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= any(~(~bitset<N>{} << i));
        check &= any(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto none() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= none(~(~bitset<N>{} << i));
        check &= none(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto count() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= count(~(~bitset<N>{} << i));
        check &= count(~bitset<N>{});
        return check;
}

// operators

template<int N>
constexpr auto op_flip() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= op_flip(~(~bitset<N>{} << i));
        check &= op_flip(~bitset<N>{});
        return check;
}

template<int N>
constexpr auto op_and() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_and(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_or() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_or(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_xor() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_xor(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_minus() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_minus(bitset<N>{i}, bitset<N>{j});
        return check;
}

template<int N>
constexpr auto op_shift_left() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_left(bitset<N>{i}, j);
        return check;
}

template<int N>
constexpr auto op_shift_right() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_right(bitset<N>{i}, j);
        return check;
}

}       // namespace xstd
