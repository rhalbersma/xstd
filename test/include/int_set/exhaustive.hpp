#pragma once
#include <int_set/primitive.hpp>
#include <xstd/int_set.hpp>              // int_set

namespace xstd {

// comparators

template<int N>
auto op_equal_to() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_equal_to(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
auto op_not_equal_to() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_not_equal_to(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
auto op_less() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_less(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
auto op_subset_of() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_subset_of(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
auto op_proper_subset_of() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_proper_subset_of(int_set<N>{i}, int_set<N>{j});
        return check;
}

// modifiers

template<int N>
constexpr auto set_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= set_one(int_set<N>{i}, j);
        return check;
}

template<int N>
constexpr auto reset_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= reset_one(int_set<N>{i}, j);
        return check;
}

template<int N>
constexpr auto flip_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= flip_one(int_set<N>{i}, j);
        return check;
}

template<int N>
auto set_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= set_all(~(~int_set<N>{} << i));
        check &= set_all(~int_set<N>{});
        return check;
}

template<int N>
auto reset_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= reset_all(~(~int_set<N>{} << i));
        check &= reset_all(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto flip_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= flip_all(~(~int_set<N>{} << i));
        check &= flip_all(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto op_and_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_and_assign(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
constexpr auto op_or_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_or_assign(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
constexpr auto op_xor_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_xor_assign(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
constexpr auto op_minus_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_minus_assign(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
auto op_shift_left_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_left_assign(int_set<N>{i}, j);
        return check;
}

template<int N>
auto op_shift_right_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_right_assign(int_set<N>{i}, j);
        return check;
}

// observers

template<int N>
auto all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= all(~(~int_set<N>{} << i));
        check &= all(~int_set<N>{});
        return check;
}

template<int N>
auto any() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= any(~(~int_set<N>{} << i));
        check &= any(~int_set<N>{});
        return check;
}

template<int N>
auto none() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= none(~(~int_set<N>{} << i));
        check &= none(~int_set<N>{});
        return check;
}

template<int N>
auto count() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= count(~(~int_set<N>{} << i));
        check &= count(~int_set<N>{});
        return check;
}

// operators

template<int N>
constexpr auto op_complement() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                check &= op_complement(~(~int_set<N>{} << i));
        check &= op_complement(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto op_and() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_and(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
constexpr auto op_or() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_or(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
constexpr auto op_xor() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_xor(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
constexpr auto op_minus() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_minus(int_set<N>{i}, int_set<N>{j});
        return check;
}

template<int N>
auto op_shift_left() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_left(int_set<N>{i}, j);
        return check;
}

template<int N>
auto op_shift_right() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i)
                for (auto j = 0; j < N; ++j)
                        check &= op_shift_right(int_set<N>{i}, j);
        return check;
}

}       // namespace xstd
