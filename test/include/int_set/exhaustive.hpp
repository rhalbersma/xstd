#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/primitive.hpp>        // primitive tests
#include <xstd/int_set.hpp>             // int_set

namespace xstd {

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
constexpr auto bitand_assign() noexcept
{
        all_singlet_pairs<N>(prim::bitand_assign{});
}

template<int N>
constexpr auto bitor_assign() noexcept
{
        all_singlet_pairs<N>(prim::bitor_assign{});
}

template<int N>
constexpr auto xor_assign() noexcept
{
        all_singlet_pairs<N>(prim::xor_assign{});
}

template<int N>
constexpr auto minus_assign() noexcept
{
        all_singlet_pairs<N>(prim::minus_assign{});
}

template<int N>
auto shift_left_assign() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::shift_left_assign(i1, 0);
                prim::shift_left_assign(i1, N - 1 - i);
                prim::shift_left_assign(i1, N - 1);
        }
}

template<int N>
auto shift_right_assign() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::shift_right_assign(i1, 0);
                prim::shift_right_assign(i1, i);
                prim::shift_right_assign(i1, N - 1);
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
                prim::erase(int_set<N>{i}, i);
        }
}

template<int N>
constexpr auto compl_() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::compl_(~(~int_set<N>{} << i));
        }
        prim::compl_(~int_set<N>{});
}

template<int N>
constexpr auto flip_all() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::flip_all(~(~int_set<N>{} << i));
        }
        prim::flip_all(~int_set<N>{});
}

template<int N>
constexpr auto flip_one() noexcept
{
        auto const i0 = int_set<N>{};
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::flip_one(i0, i);
                prim::flip_one(i1, i);
        }
}

template<int N>
auto size_() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::size_(~(~int_set<N>{} << i));
        }
        prim::size_(~int_set<N>{});
}

template<int N>
auto non_member_size() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::non_member_size(~(~int_set<N>{} << i));
        }
        prim::non_member_size(~int_set<N>{});
}

template<int N>
auto max_size() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::max_size(~(~int_set<N>{} << i));
        }
        prim::max_size(~int_set<N>{});
}

template<int N>
auto equal_to() noexcept
{
        all_singlet_pairs<N>(prim::equal_to{});
}

template<int N>
auto not_equal_to() noexcept
{
        all_singlet_pairs<N>(prim::not_equal_to{});
}

template<int N>
auto less() noexcept
{
        all_singlet_pairs<N>(prim::less{});
}

template<int N>
auto greater() noexcept
{
        all_singlet_pairs<N>(prim::greater{});
}

template<int N>
auto greater_equal() noexcept
{
        all_singlet_pairs<N>(prim::greater_equal{});
}

template<int N>
auto less_equal() noexcept
{
        all_singlet_pairs<N>(prim::less_equal{});
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
auto full() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::full(~(~int_set<N>{} << i));
        }
        prim::full(~int_set<N>{});
}

template<int N>
auto not_empty() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::not_empty(~(~int_set<N>{} << i));
        }
        prim::not_empty(~int_set<N>{});
}

template<int N>
auto empty_() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::empty_(~(~int_set<N>{} << i));
        }
        prim::empty_(~int_set<N>{});
}

template<int N>
auto non_member_empty() noexcept
{
        for (auto i = 0; i < N; ++i) {
                prim::non_member_empty(~(~int_set<N>{} << i));
        }
        prim::non_member_empty(~int_set<N>{});
}

// operators

template<int N>
auto shift_left() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::shift_left(i1, 0);
                prim::shift_left(i1, N - 1 - i);
                prim::shift_left(i1, N - 1);
        }
}

template<int N>
auto shift_right() noexcept
{
        for (auto i = 0; i < N; ++i) {
                auto const i1 = int_set<N>{i};
                prim::shift_right(i1, 0);
                prim::shift_right(i1, i);
                prim::shift_right(i1, N - 1);
        }
}

template<int N>
constexpr auto bitand_() noexcept
{
        all_singlet_pairs<N>(prim::bitand_{});
}

template<int N>
constexpr auto bitor_() noexcept
{
        all_singlet_pairs<N>(prim::bitor_{});
}

template<int N>
constexpr auto xor_() noexcept
{
        all_singlet_pairs<N>(prim::xor_{});
}

template<int N>
constexpr auto minus() noexcept
{
        all_singlet_pairs<N>(prim::minus{});
}

}       // namespace xstd
