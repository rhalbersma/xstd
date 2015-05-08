#pragma once
#include <xstd/array.hpp>                       // array
#include <xstd/bit_array/bit_array_fwd.hpp>     // bit_array
#include <xstd/bit/mask.hpp>                    // none, one, all
#include <xstd/bit/primitive.hpp>               // popcount
#include <xstd/cstddef.hpp>                     // _z
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <cassert>                              // assert
#include <cstddef>                              // size_t
#include <type_traits>                          // enable_if
#include <utility>                              // swap

namespace xstd {
namespace bit {

template<class Block, std::size_t Nb>
struct bit_array
{
        static_assert(is_unsigned_integer<Block>, "");
        static constexpr auto N = Nb * digits<Block>;

        array<Block, Nb> elems {};

        // implicit
        constexpr bit_array(Block value) noexcept
        :
                elems{value}
        {}

        constexpr bit_array() = default;

        // data access

        constexpr auto* block_begin() noexcept
        {
                return elems.begin();
        }

        constexpr auto const* block_begin() const noexcept
        {
                return elems.begin();
        }

        constexpr auto* block_end() noexcept
        {
                return elems.end();
        }

        constexpr auto const* block_end() const noexcept
        {
                return elems.end();
        }

        constexpr auto& block_back() noexcept
        {
                return elems.back();
        }

        constexpr auto const& block_back() const noexcept
        {
                return elems.back();
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

        constexpr auto do_equal(bit_array const& other) const noexcept
        {
                return elems == other.elems;
        }

        constexpr auto do_colexicographical_compare(bit_array const& other) const noexcept
        {
                return xstd::lexicographical_compare(elems.rbegin(), elems.rend(), other.elems.rbegin(), other.elems.rend());
        }

        constexpr auto do_intersects(bit_array const& other) const noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        if (elems[i] & other.elems[i])
                                return true;
                return false;
        }

        constexpr auto do_is_subset_of(bit_array const& other) const noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        if (elems[i] & ~other.elems[i])
                                return false;
                return true;
        }

        constexpr auto do_is_proper_subset_of(bit_array const& other) const noexcept
        {
                auto proper = false;
                for (auto i = 0_z; i < Nb; ++i) {
                        if ( elems[i] & ~other.elems[i])
                                return false;
                        if (~elems[i] &  other.elems[i])
                                proper = true;
                }
                return proper;
        }

        // modifiers

        constexpr auto do_swap(bit_array& other) noexcept
        {
                elems.swap(other.elems);
        }

        constexpr auto do_set() noexcept
        {
                elems.fill(mask::all<Block>);
        }

        constexpr auto do_reset() noexcept
        {
                elems.fill(mask::none<Block>);
        }

        constexpr auto do_flip() noexcept
        {
                for (auto& block : elems)
                        block = ~block;
        }

        constexpr auto do_and(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] &= other.elems[i];
        }

        constexpr auto do_or(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] |= other.elems[i];
        }

        constexpr auto do_xor(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] ^= other.elems[i];
        }

        constexpr auto do_minus(bit_array const& other) noexcept
        {
                for (auto i = 0_z; i < Nb; ++i)
                        elems[i] &= ~other.elems[i];
        }

        constexpr auto do_left_shift(std::size_t n)
        {
                assert(n < N);
                if (n == 0) return;

                auto const n_block = n / digits<Block>;
                auto const L_shift = n % digits<Block>;

                if (L_shift == 0) {
                        for (auto i = Nb - 1; i >= n_block; --i)
                                elems[i] = elems[i - n_block];
                } else {
                        auto const R_shift = digits<Block> - L_shift;

                        for (auto i = Nb - 1; i > n_block; --i)
                                elems[i] =
                                        (elems[i - n_block    ] << L_shift) |
                                        (elems[i - n_block - 1] >> R_shift)
                                ;
                        elems[n_block] = elems[0] << L_shift;
                }
                xstd::fill_n(&elems[0], n_block, mask::none<Block>);
        }

        constexpr auto do_right_shift(std::size_t n)
        {
                assert(n < N);
                if (n == 0) return;

                auto const n_block = n / digits<Block>;
                auto const R_shift = n % digits<Block>;

                if (R_shift == 0) {
                        for (auto i = 0_z; i <= Nb - 1 - n_block; ++i)
                               elems[i] = elems[i + n_block];
                } else {
                        auto const L_shift = digits<Block> - R_shift;

                        for (auto i = 0_z; i < Nb - 1 - n_block; ++i)
                                elems[i] =
                                        (elems[i + n_block    ] >> R_shift) |
                                        (elems[i + n_block + 1] << L_shift)
                                ;
                        elems[Nb - 1 - n_block] = elems[Nb - 1] >> R_shift;
                }
                xstd::fill_n(&elems[0] + Nb - n_block, n_block, mask::none<Block>);
        }

        // observers

        template<std::size_t M>
        constexpr std::enable_if_t<M != 0,
        bool> do_all() const noexcept
        {
                static_assert(0 < M && M < digits<Block>, "");
                for (auto i = 0_z; i < Nb - 1; ++i)
                        if (elems[i] != mask::all<Block>)
                                return false;
                return elems[Nb - 1] == mask::all<Block> >> (digits<Block> - M);
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M == 0,
        bool> do_all() const noexcept
        {
                for (auto const& block : elems)
                        if (block != mask::all<Block>)
                                return false;
                return true;
        }

        constexpr auto do_any() const noexcept
        {
                for (auto const& block : elems)
                        if (block != mask::none<Block>)
                                return true;
                return false;
        }

        constexpr auto do_none() const noexcept
        {
                for (auto const& block : elems)
                        if (block != mask::none<Block>)
                                return false;
                return true;
        }

        constexpr auto do_count() const noexcept
        {
                auto sum = 0_z;
                for (auto const& block : elems)
                        sum += popcount(block);
                return sum;
        }
};

}       // namespace bit
}       // namespace xstd
