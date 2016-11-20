#pragma once
#include <xstd/bit/detail/base_bitset_fwd.hpp>  // base_bitset
#include <xstd/bit/mask.hpp>                    // none, one, all
#include <xstd/bit/primitive.hpp>               // ctznz, popcount
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <boost/algorithm/cxx11/all_of.hpp>     // all_of
#include <boost/algorithm/cxx11/any_of.hpp>     // any_of
#include <boost/algorithm/cxx11/none_of.hpp>    // none_of
#include <boost/range/adaptors.hpp>             // reverse
#include <boost/range/algorithm.hpp>            // equal, fill_n, lexicographical_compare
#include <boost/range/algorithm/swap_ranges.hpp>// swap_ranges
#include <boost/range/combine.hpp>              // combine
#include <boost/range/numeric.hpp>              // accumulate
#include <boost/tuple/tuple.hpp>                // make_tuple
#include <algorithm>                            // all_of, copy_n, copy_backward
#include <cassert>                              // assert
#include <iterator>                             // begin, end, cbegin, cend, crbegin, crend
#include <utility>                              // move

namespace xstd {
namespace bit {
namespace detail {

template<class Block, int Nb>
struct base_bitset
{
        static_assert(is_unsigned_integer<Block>);
        static constexpr auto N = Nb * digits<Block>;

        Block elems[Nb];

        base_bitset() = default;

        /* implicit */ constexpr base_bitset(Block value) noexcept
        :
                elems{{value}}
        {}

        // data access

        constexpr auto* block_begin() noexcept
        {
                using std::begin;
                return begin(elems);
        }

        constexpr auto const* block_begin() const noexcept
        {
                using std::begin;
                return begin(elems);
        }

        constexpr auto* block_end() noexcept
        {
                using std::end;
                return end(elems);
        }

        constexpr auto const* block_end() const noexcept
        {
                using std::end;
                return end(elems);
        }

        constexpr auto& block_back() noexcept
        {
                return elems[Nb - 1];
        }

        constexpr auto const& block_back() const noexcept
        {
                return elems[Nb - 1];
        }

        constexpr auto& block_ref(int n)
        {
                assert(n < N);
                return elems[n / digits<Block>];
        }

        constexpr auto const& block_ref(int n) const
        {
                assert(n < N);
                return elems[n / digits<Block>];
        }

        constexpr auto block_mask(int n) const noexcept
        {
                assert(n < N);
                return mask::one<Block> << (n % digits<Block>);
        }

        // comparators

        auto op_equal_to(base_bitset const& other) const noexcept
        {
                return boost::equal(elems, other.elems);
        }

        auto op_less(base_bitset const& other) const noexcept
        {
                using boost::adaptors::reverse;
                return boost::lexicographical_compare(reverse(elems), reverse(other.elems));
        }

        auto do_intersects(base_bitset const& other) const noexcept
        {
                return boost::algorithm::any_of(boost::combine(elems, other.elems), [](auto const block){
                        return boost::get<0>(block) & boost::get<1>(block);
                });

        }

        auto do_is_subset_of(base_bitset const& other) const noexcept
        {
                return boost::algorithm::all_of(boost::combine(elems, other.elems), [](auto const block){
                        return !(boost::get<0>(block) & ~boost::get<1>(block));
                });
        }

        // modifiers

        auto do_swap(base_bitset& other) noexcept
        {
                boost::range::swap_ranges(elems, other.elems);
        }

        auto do_set() noexcept
        {
                boost::fill_n(elems, Nb, mask::all<Block>);
        }

        auto do_reset() noexcept
        {
                boost::fill_n(elems, Nb, mask::none<Block>);
        }

        constexpr auto op_flip() noexcept
        {
                for (auto& block : elems)
                        block = ~block;
        }

        constexpr auto op_and(base_bitset const& other) noexcept
        {
                for (auto i = 0; i < Nb; ++i)
                        elems[i] &= other.elems[i];
        }

        constexpr auto op_or(base_bitset const& other) noexcept
        {
                for (auto i = 0; i < Nb; ++i)
                        elems[i] |= other.elems[i];
        }

        constexpr auto op_xor(base_bitset const& other) noexcept
        {
                for (auto i = 0; i < Nb; ++i)
                        elems[i] ^= other.elems[i];
        }

        constexpr auto op_minus(base_bitset const& other) noexcept
        {
                for (auto i = 0; i < Nb; ++i)
                        elems[i] &= ~other.elems[i];
        }

        auto op_left_shift(int const n)
        {
                assert(n < N);
                using std::begin; using std::end;
                if (n == 0) return;

                auto const n_block = n / digits<Block>;
                auto const L_shift = n % digits<Block>;

                if (L_shift == 0) {
                        std::copy_backward(begin(elems), end(elems) - n_block, end(elems));
                } else {
                        auto const R_shift = digits<Block> - L_shift;

                        for (auto i = Nb - 1; i > n_block; --i) {
                                elems[i] =
                                        (elems[i - n_block    ] << L_shift) |
                                        (elems[i - n_block - 1] >> R_shift)
                                ;
                        }
                        elems[n_block] = elems[0] << L_shift;
                }
                boost::fill_n(elems, n_block, mask::none<Block>);
        }

        auto op_right_shift(int const n)
        {
                assert(n < N);
                using std::begin; using std::end;
                using boost::adaptors::reverse;
                if (n == 0) return;

                auto const n_block = n / digits<Block>;
                auto const R_shift = n % digits<Block>;

                if (R_shift == 0) {
                        std::copy_n(begin(elems) + n_block, Nb - n_block, begin(elems));
                } else {
                        auto const L_shift = digits<Block> - R_shift;

                        for (auto i = 0; i < Nb - 1 - n_block; ++i) {
                                elems[i] =
                                        (elems[i + n_block    ] >> R_shift) |
                                        (elems[i + n_block + 1] << L_shift)
                                ;
                        }
                        elems[Nb - 1 - n_block] = elems[Nb - 1] >> R_shift;
                }
                boost::fill_n(reverse(elems), n_block, mask::none<Block>);
        }

        template<class UnaryFunction>
        constexpr auto do_for_each(UnaryFunction f) const
        {
                for (auto i = 0, offset = 0; i < Nb; ++i, offset += digits<Block>) {
                        for (auto block = elems[i]; block;) {
                                auto const first = bsfnz(block);
                                f(offset + first);
                                block ^= block_mask(first);
                        }
                }
                return std::move(f);
        }

        template<class UnaryFunction>
        constexpr auto do_reverse_for_each(UnaryFunction f) const
        {
                for (auto i = Nb - 1, offset = N - digits<Block>; i >= 0; --i, offset -= digits<Block>) {
                        for (auto block = elems[i]; block;) {
                                auto const last = bsrnz(block);
                                f(offset + last);
                                block ^= block_mask(last);
                        }
                }
                return std::move(f);
        }

        // observers

        template<int M>
        auto do_all() const noexcept
        {
                static_assert(M < digits<Block>);
                if constexpr (M != 0) {
                        using std::cbegin; using std::cend;
                        return std::all_of(cbegin(elems), cend(elems) - 1, [](auto const block){
                                return block == mask::all<Block>;
                        }) && (elems[Nb - 1] == mask::all<Block> >> (digits<Block> - M));
                } else {
                        return boost::algorithm::all_of(elems, [](auto const block){
                                return block == mask::all<Block>;
                        });
                }
        }

        auto do_any() const noexcept
        {
                return boost::algorithm::any_of(elems, [](auto const block){
                        return block != mask::none<Block>;
                });
        }

        auto do_none() const noexcept
        {
                return boost::algorithm::none_of(elems, [](auto const block){
                        return block != mask::none<Block>;
                });
        }

        auto do_count() const noexcept
        {
                return boost::accumulate(elems, 0, [](auto const sum, auto const block){
                        return sum + popcount(block);
                });
        }
};

}       // namespace detail
}       // namespace bit
}       // namespace xstd
