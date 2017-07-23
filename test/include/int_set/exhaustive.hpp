#pragma once
#include <bitset>       // bitset
#include <cassert>      // assert
#include <type_traits>  // is_same_v

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

namespace xstd {

// NOTE: these tests are O(N)

template<class IntSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
auto all_counts(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto const ii = ~(~IntSet{} << i);
                fun(ii);
        }
        auto const iN = ~IntSet{};
        fun(iN);
}

template<class IntSet, class UnaryFunction>
auto all_singlets(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto const i1 = IntSet{}.set(i);
                fun(i1);
        }
}

// NOTE: this test is O(N^2)

template<class IntSet, class BinaryFunction>
auto all_singlet_pairs(BinaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto const i1 = IntSet{}.set(i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        auto const j1 = IntSet{}.set(j);
                        fun(i1, j1);
                }
        }
}

// NOTE: this test is O(N^3)

template<class IntSet, class UnaryFunction>
auto all_pairs(UnaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N - 1; ++i) {
                for (auto j = SizeType{i + 1}; j < N; ++j) {
                        auto const ij2 = IntSet{}.set(i).set(j);
                        fun(ij2);
                }
        }
}

template<class IntSet, class TernaryFunction>
auto all_singlet_triples(TernaryFunction fun)
{
        constexpr auto N = IntSet{}.size();
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto const i1 = IntSet{}.set(i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        auto const j1 = IntSet{}.set(j);
                        for (auto k = SizeType{0}; k < N; ++k) {
                                auto const k1 = IntSet{}.set(k);
                                fun(i1, j1, k1);
                        }
                }
        }
}

}       // namespace xstd
