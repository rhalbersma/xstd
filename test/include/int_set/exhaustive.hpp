#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <int_set/primitive.hpp>        // primitive tests
#include <xstd/int_set.hpp>             // int_set
#include <type_traits>                  // decay_t

namespace xstd {

template<class IntSet, class UnaryFunction>
constexpr auto all_values(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = std::decay_t<decltype(N)>;
        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
constexpr auto all_counts(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = std::decay_t<decltype(N)>;
        for (auto i = SizeType{0}; i < N; ++i) {
                auto const ii = ~(~IntSet{} << i);
                fun(ii);
        }
        auto const iN = ~IntSet{};
        fun(iN);
}

template<class IntSet, class UnaryFunction>
constexpr auto all_singlets(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = std::decay_t<decltype(N)>;
        for (auto i = SizeType{0}; i < N; ++i) {
                auto const i1 = IntSet{i};
                fun(i1);
        }
}

// NOTE: this test is O(N^2)
template<class IntSet, class BinaryFunction>
constexpr auto all_singlet_pairs(BinaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = std::decay_t<decltype(N)>;
        auto const i0 = IntSet{};
        fun(i0, i0);
        for (auto i = SizeType{0}; i < N; ++i) {
                auto const i1 = IntSet{i};
                fun(i0, i1);
                fun(i1, i0);
                for (auto j = SizeType{0}; j < N; ++j) {
                        auto const j1 = IntSet{j};
                        fun(i1, j1);
                }
        }
}

template<class T>
constexpr auto constructor() noexcept
{
        prim::constructor<T>{}();
}

template<class T>
constexpr auto op_bitand_assign() noexcept
{
        all_singlet_pairs<T>(prim::op_bitand_assign{});
}

template<class T>
constexpr auto op_bitor_assign() noexcept
{
        all_singlet_pairs<T>(prim::op_bitor_assign{});
}

template<class T>
constexpr auto op_xor_assign() noexcept
{
        all_singlet_pairs<T>(prim::op_xor_assign{});
}

template<class T>
constexpr auto op_minus_assign() noexcept
{
        all_singlet_pairs<T>(prim::op_minus_assign{});
}

template<class T>
auto op_shift_left_assign() noexcept
{
        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        prim::op_shift_left_assign{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        prim::op_shift_left_assign{}(i1, pos);
                });
        });
}

template<class T>
auto op_shift_right_assign() noexcept
{
        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        prim::op_shift_right_assign{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        prim::op_shift_right_assign{}(i1, pos);
                });
        });
}

template<class T>
auto set() noexcept
{
        all_counts<T>(prim::set{});
        all_values<T>([](auto const pos) {
                prim::set{}(T{}, pos);
        });
}

template<class T>
auto reset() noexcept
{
        all_counts<T>(prim::reset{});
        all_values<T>([](auto const pos) {
                prim::reset{}(~T{}, pos);
        });
}

template<class T>
constexpr auto op_compl() noexcept
{
        all_counts<T>(prim::op_compl{});
        all_singlets<T>(prim::op_compl{});
        all_singlet_pairs<T>(prim::op_compl{});
}

template<class T>
constexpr auto flip() noexcept
{
        all_counts<T>(prim::flip{});
        all_values<T>([](auto const pos) {
                prim::flip{}(T{}, pos);
                all_singlets<T>([&](auto const& i1) {
                        prim::flip{}(i1, pos);
                });
        });
}

template<class T>
auto count_() noexcept
{
        all_counts<T>(prim::count_{});
}

template<class T>
auto for_each_() noexcept
{
        all_counts<T>(prim::for_each_{});
}

template<class T>
auto reverse_for_each_() noexcept
{
        all_counts<T>(prim::reverse_for_each_{});
}

template<class T>
auto size() noexcept
{
        all_counts<T>(prim::size{});
}

template<class T>
auto op_equal_to() noexcept
{
        all_singlet_pairs<T>(prim::op_equal_to{});
}

template<class T>
auto op_not_equal_to() noexcept
{
        all_singlet_pairs<T>(prim::op_not_equal_to{});
}

template<class T>
auto op_less() noexcept
{
        all_singlet_pairs<T>(prim::op_less{});
}

template<class T>
auto op_greater() noexcept
{
        all_singlet_pairs<T>(prim::op_greater{});
}

template<class T>
auto op_greater_equal() noexcept
{
        all_singlet_pairs<T>(prim::op_greater_equal{});
}

template<class T>
auto op_less_equal() noexcept
{
        all_singlet_pairs<T>(prim::op_less_equal{});
}

template<class T>
auto is_subset_of_() noexcept
{
        all_singlet_pairs<T>(prim::is_subset_of_{});
}

template<class T>
auto is_superset_of_() noexcept
{
        all_singlet_pairs<T>(prim::is_superset_of_{});
}

template<class T>
auto is_proper_subset_of_() noexcept
{
        all_singlet_pairs<T>(prim::is_proper_subset_of_{});
}

template<class T>
auto is_proper_superset_of_() noexcept
{
        all_singlet_pairs<T>(prim::is_proper_superset_of_{});
}

template<class T>
constexpr auto test() noexcept
{
        prim::test{}(T{});
        all_values<T>([](auto const pos) {
                prim::test{}(T{}, pos);
        });
}

template<class T>
auto all() noexcept
{
        all_counts<T>(prim::all{});
}

template<class T>
auto any() noexcept
{
        all_counts<T>(prim::any{});
}

template<class T>
auto none() noexcept
{
        all_counts<T>(prim::none{});
}

// operators

template<class T>
auto op_shift_left() noexcept
{
        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        prim::op_shift_left{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        prim::op_shift_left{}(i1, pos);
                });
        });
}

template<class T>
auto op_shift_right() noexcept
{
        all_values<T>([](auto const pos){
                all_counts<T>([&](auto const& ii){
                        prim::op_shift_right{}(ii, pos);
                });
                all_singlets<T>([&](auto const& i1){
                        prim::op_shift_right{}(i1, pos);
                });
        });
}

template<class T>
constexpr auto op_bitand() noexcept
{
        all_counts<T>(prim::op_bitand{});
        all_singlets<T>(prim::op_bitand{});
        all_singlet_pairs<T>(prim::op_bitand{});
}

template<class T>
constexpr auto op_bitor() noexcept
{
        all_counts<T>(prim::op_bitor{});
        all_singlets<T>(prim::op_bitor{});
        all_singlet_pairs<T>(prim::op_bitor{});
}

template<class T>
constexpr auto op_xor() noexcept
{
        all_counts<T>(prim::op_xor{});
        all_singlets<T>(prim::op_xor{});
        all_singlet_pairs<T>(prim::op_xor{});
}

template<class T>
constexpr auto op_minus() noexcept
{
        all_counts<T>(prim::op_minus{});
        all_singlets<T>(prim::op_minus{});
        all_singlet_pairs<T>(prim::op_minus{});
}

}       // namespace xstd
