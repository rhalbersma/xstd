#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/primitive.hpp>        // primitive tests
#include <xstd/int_set.hpp>             // int_set

namespace xstd {

template<int N, class Test>
constexpr auto all_counts(Test test)
{
        for (auto i = 0; i < N; ++i) {
                auto const ii = ~(~int_set<N>{} << i);
                test(ii);
        }
        auto const iN = ~int_set<N>{};
        test(iN);
}

template<int N, class Test>
constexpr auto all_singlets(Test test)
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                test(i1);
        }
}

// NOTE: this test is O(N^2)
template<int N, class Test>
constexpr auto all_singlet_pairs(Test test)
{
        auto const i0 = int_set<N>{};
        test(i0, i0);
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                test(i0, i1);
                test(i1, i0);
                for (auto j = 0; j < N; ++j) {
                        auto const j1 = int_set<N>{j};
                        test(i1, j1);
                }
        }
}

template<int N>
constexpr auto op_bitand_assign() noexcept
{
        all_singlet_pairs<N>(prim::op_bitand_assign{});
}

template<int N>
constexpr auto op_bitor_assign() noexcept
{
        all_singlet_pairs<N>(prim::op_bitor_assign{});
}

template<int N>
constexpr auto op_xor_assign() noexcept
{
        all_singlet_pairs<N>(prim::op_xor_assign{});
}

template<int N>
constexpr auto op_minus_assign() noexcept
{
        all_singlet_pairs<N>(prim::op_minus_assign{});
}

template<int N>
auto op_shift_left_assign() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::op_shift_left_assign(i1, 0);
                prim::op_shift_left_assign(i1, N - 1 - i);
                prim::op_shift_left_assign(i1, N - 1);
        }
}

template<int N>
auto op_shift_right_assign() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::op_shift_right_assign(i1, 0);
                prim::op_shift_right_assign(i1, i);
                prim::op_shift_right_assign(i1, N - 1);
        }
}

template<int N>
auto fill() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::fill(~(~int_set<N>{} << i));
        }
        prim::fill(~int_set<N>{});
}

template<int N>
constexpr auto insert() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::insert(int_set<N>{}, i);
        }
}

template<int N>
auto clear() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::clear(~(~int_set<N>{} << i));
        }
        prim::clear(~int_set<N>{});
}

template<int N>
constexpr auto erase() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto i1 = int_set<N>{i};
                prim::erase(i1, i);
        }

        auto i0 = int_set<N>{};
        prim::erase(i0, i0.begin(), i0.end());
        for (auto i = 0; i < N; ++i) {
                auto i1 = int_set<N>{i};
                prim::erase(i1, i1.begin(), i1.end());
        }

        for (auto i = 0; i < N; ++i) {
                auto const ilist{i};
                auto i1 = int_set<N>{ilist};
                prim::erase(i1, ilist);
        }
}

template<int N>
constexpr auto op_compl() noexcept
{
        all_counts<N>(prim::op_compl{});
        all_singlets<N>(prim::op_compl{});
        all_singlet_pairs<N>(prim::op_compl{});
}

template<int N>
constexpr auto toggle_all() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::toggle_all(~(~int_set<N>{} << i));
        }
        prim::toggle_all(~int_set<N>{});
}

template<int N>
constexpr auto toggle_one() noexcept
{
        auto const i0 = int_set<N>{};
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::toggle_one(i0, i);
                prim::toggle_one(i1, i);
        }
}

template<int N>
auto count_() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::count_(~(~int_set<N>{} << i));
        }
        prim::count_(~int_set<N>{});
}

template<int N>
auto for_each_() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::for_each_(~(~int_set<N>{} << i));
        }
        prim::for_each_(~int_set<N>{});
}

template<int N>
auto reverse_for_each_() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::reverse_for_each_(~(~int_set<N>{} << i));
        }
        prim::reverse_for_each_(~int_set<N>{});
}

template<int N>
auto size() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::size{}(~(~int_set<N>{} << i));
        }
        prim::size{}(~int_set<N>{});
}

template<int N>
auto op_equal_to() noexcept
{
        all_singlet_pairs<N>(prim::op_equal_to{});
}

template<int N>
auto op_not_equal_to() noexcept
{
        all_singlet_pairs<N>(prim::op_not_equal_to{});
}

template<int N>
auto op_less() noexcept
{
        all_singlet_pairs<N>(prim::op_less{});
}

template<int N>
auto op_greater() noexcept
{
        all_singlet_pairs<N>(prim::op_greater{});
}

template<int N>
auto op_greater_equal() noexcept
{
        all_singlet_pairs<N>(prim::op_greater_equal{});
}

template<int N>
auto op_less_equal() noexcept
{
        all_singlet_pairs<N>(prim::op_less_equal{});
}

template<int N>
auto is_subset_of_() noexcept
{
        all_singlet_pairs<N>(prim::is_subset_of_{});
}

template<int N>
auto is_superset_of_() noexcept
{
        all_singlet_pairs<N>(prim::is_superset_of_{});
}

template<int N>
auto is_proper_subset_of_() noexcept
{
        all_singlet_pairs<N>(prim::is_proper_subset_of_{});
}

template<int N>
auto is_proper_superset_of_() noexcept
{
        all_singlet_pairs<N>(prim::is_proper_superset_of_{});
}

template<int N>
constexpr auto test() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::test<N>(i);
        }
}

template<int N>
auto all() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::all(~(~int_set<N>{} << i));
        }
        prim::all(~int_set<N>{});
}

template<int N>
auto any() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::any(~(~int_set<N>{} << i));
        }
        prim::any(~int_set<N>{});
}

template<int N>
auto none() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::none(~(~int_set<N>{} << i));
        }
        prim::none(~int_set<N>{});
}

// operators

template<int N>
auto op_shift_left() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::op_shift_left(i1, 0);
                prim::op_shift_left(i1, N - 1 - i);
                prim::op_shift_left(i1, N - 1);
        }
}

template<int N>
auto op_shift_right() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::op_shift_right(i1, 0);
                prim::op_shift_right(i1, i);
                prim::op_shift_right(i1, N - 1);
        }
}

template<int N>
constexpr auto op_bitand() noexcept
{
        all_counts<N>(prim::op_bitand{});
        all_singlets<N>(prim::op_bitand{});
        all_singlet_pairs<N>(prim::op_bitand{});
}

template<int N>
constexpr auto op_bitor() noexcept
{
        all_counts<N>(prim::op_bitor{});
        all_singlets<N>(prim::op_bitor{});
        all_singlet_pairs<N>(prim::op_bitor{});
}

template<int N>
constexpr auto op_xor() noexcept
{
        all_counts<N>(prim::op_xor{});
        all_singlets<N>(prim::op_xor{});
        all_singlet_pairs<N>(prim::op_xor{});
}

template<int N>
constexpr auto op_minus() noexcept
{
        all_counts<N>(prim::op_minus{});
        all_singlets<N>(prim::op_minus{});
        all_singlet_pairs<N>(prim::op_minus{});
}

}       // namespace xstd
