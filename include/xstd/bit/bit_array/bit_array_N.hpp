#pragma once
#include <xstd/bit/bit_array/bit_array_fwd.hpp> // bit_array
#include <xstd/bit/mask.hpp>                    // none, one, all
#include <xstd/bit/primitive.hpp>               // popcount
#include <xstd/cstddef.hpp>                     // _z
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <boost/iterator/zip_iterator.hpp>      // make_zip_iterator
#include <boost/tuple/tuple.hpp>                // make_tuple
#include <algorithm>                            // all_of, any_of, none_of, equal, lexicographical_compare,
                                                // copy, copy_backward, fill_n, swap_ranges
#include <cassert>                              // assert
#include <cstddef>                              // size_t
#include <iterator>                             // begin, end, cbegin, cend, crbegin, crend
#include <numeric>                              // accumulate
#include <type_traits>                          // enable_if

namespace xstd {
namespace bit {

template<class Block, std::size_t Nb>
struct bit_array
{
        static_assert(is_unsigned_integer<Block>, "");
        static constexpr auto N = Nb * digits<Block>;

        Block elems[Nb];

        bit_array() = default;

        /* implicit */ constexpr bit_array(Block value) noexcept
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

        constexpr auto& block_ref(std::size_t n)
        {
                assert(n < N);
                return elems[n / digits<Block>];
        }

        constexpr auto const& block_ref(std::size_t n) const
        {
                assert(n < N);
                return elems[n / digits<Block>];
        }

        // comparators

        auto op_equal_to(bit_array const& other) const noexcept
        {
                using std::cbegin; using std::cend;
                return std::equal(cbegin(elems), cend(elems), cbegin(other.elems), cend(other.elems));
        }

        auto op_less(bit_array const& other) const noexcept
        {
                using std::crbegin; using std::crend;
                return std::lexicographical_compare(crbegin(elems), crend(elems), crbegin(other.elems), crend(other.elems));
        }

        auto do_intersects(bit_array const& other) const noexcept
        {
                using std::cbegin; using std::cend;
                return std::any_of(
                        boost::make_zip_iterator(boost::make_tuple(cbegin(elems), cbegin(other.elems))),
                        boost::make_zip_iterator(boost::make_tuple(cend  (elems), cend  (other.elems))),
                        [](auto const& blocks){
                        return boost::get<0>(blocks) & boost::get<1>(blocks);
                });
        }

        auto do_is_subset_of(bit_array const& other) const noexcept
        {
                using std::cbegin; using std::cend;
                return std::all_of(
                        boost::make_zip_iterator(boost::make_tuple(cbegin(elems), cbegin(other.elems))),
                        boost::make_zip_iterator(boost::make_tuple(cend  (elems), cend  (other.elems))),
                        [](auto const& blocks){
                        return !(boost::get<0>(blocks) & ~boost::get<1>(blocks));
                });
        }

        // modifiers

        auto do_swap(bit_array& other) noexcept
        {
                using std::begin; using std::end;
                std::swap_ranges(begin(elems), end(elems), begin(other.elems));
        }

        auto do_set() noexcept
        {
                using std::begin;
                std::fill_n(begin(elems), Nb, mask::all<Block>);
        }

        auto do_reset() noexcept
        {
                using std::begin;
                std::fill_n(begin(elems), Nb, mask::none<Block>);
        }

        constexpr auto op_flip() noexcept
        {
                for (auto& block : elems)
                        block = ~block;
        }

        constexpr auto op_and(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] &= other.elems[i];
        }

        constexpr auto op_or(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] |= other.elems[i];
        }

        constexpr auto op_xor(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] ^= other.elems[i];
        }

        constexpr auto op_minus(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] &= ~other.elems[i];
        }

        auto op_left_shift(std::size_t n)
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

                        for (auto i = Nb - 1; i > n_block; --i)
                                elems[i] =
                                        (elems[i - n_block    ] << L_shift) |
                                        (elems[i - n_block - 1] >> R_shift)
                                ;
                        elems[n_block] = elems[0] << L_shift;
                }
                std::fill_n(begin(elems), n_block, mask::none<Block>);
        }

        auto op_right_shift(std::size_t n)
        {
                assert(n < N);
                using std::begin; using std::end;
                if (n == 0) return;

                auto const n_block = n / digits<Block>;
                auto const R_shift = n % digits<Block>;

                if (R_shift == 0) {
                        std::copy(begin(elems) + n_block, end(elems), begin(elems));
                } else {
                        auto const L_shift = digits<Block> - R_shift;

                        for (auto i = 0_z; i < Nb - 1 - n_block; ++i)
                                elems[i] =
                                        (elems[i + n_block    ] >> R_shift) |
                                        (elems[i + n_block + 1] << L_shift)
                                ;
                        elems[Nb - 1 - n_block] = elems[Nb - 1] >> R_shift;
                }
                std::fill_n(end(elems) - n_block, n_block, mask::none<Block>);
        }

        // observers

        template<std::size_t M>
        auto do_all() const noexcept
                -> std::enable_if_t<M != 0, bool>
        {
                static_assert(M < digits<Block>, "");
                using std::cbegin; using std::cend;
                return std::all_of(cbegin(elems), cend(elems) - 1,
                        [](auto const& block){
                        return block == mask::all<Block>;
                }) ? elems[Nb - 1] == mask::all<Block> >> (digits<Block> - M) : false;
        }

        template<std::size_t M>
        auto do_all() const noexcept
                -> std::enable_if_t<M == 0, bool>
        {
                using std::cbegin; using std::cend;
                return std::all_of(cbegin(elems), cend(elems),
                        [](auto const& block){
                        return block == mask::all<Block>;
                });
        }

        auto do_any() const noexcept
        {
                using std::cbegin; using std::cend;
                return std::any_of(cbegin(elems), cend(elems),
                        [](auto const& block){
                        return block != mask::none<Block>;
                });
        }

        auto do_none() const noexcept
        {
                using std::cbegin; using std::cend;
                return std::none_of(cbegin(elems), cend(elems),
                        [](auto const& block){
                        return block != mask::none<Block>;
                });
        }

        auto do_count() const noexcept
        {
                using std::cbegin; using std::cend;
                return std::accumulate(cbegin(elems), cend(elems), 0_z,
                        [](auto const& sum, auto const& block){
                        return sum + popcount(block);
                });
        }
};

}       // namespace bit
}       // namespace xstd
