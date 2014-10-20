#pragma once
#include <xstd/array.hpp>                       // array
#include <xstd/bit_array/bit_array_fwd.hpp>     // bit_array
#include <xstd/bitset/intrinsic.hpp>            // popcount
#include <xstd/bitset/limits.hpp>               // digits, is_unsigned_integer
#include <xstd/bitset/masks.hpp>                // none, one, all
#include <xstd/cstddef.hpp>                     // _z
#include <cassert>                              // assert
#include <cstddef>                              // size_t
#include <type_traits>                          // enable_if_t
#include <utility>                              // swap

namespace xstd {

template<class Block, std::size_t Nb>
class bit_array
{
private:
        static_assert(is_unsigned_integer<Block>, "");
        static constexpr auto N = Nb * digits<Block>;

public:
        // constructors

        constexpr bit_array() = default;

        /* implicit */ constexpr bit_array(Block value) noexcept
        :
                elems{value}
        {}

protected:
        // destructor

        ~bit_array() = default;

public:
        // copying, moving and assignment

        bit_array(bit_array const&) = default;
        bit_array(bit_array&&) = default;
        bit_array& operator=(bit_array const&) = default;
        bit_array& operator=(bit_array&&) = default;

        // data access

        constexpr auto* block_begin() noexcept
        {
                return &elems[0];
        }

        constexpr auto const* block_begin() const noexcept
        {
                return &elems[0];
        }

        constexpr auto* block_end() noexcept
        {
                return &elems[0] + Nb;
        }

        constexpr auto const* block_end() const noexcept
        {
                return &elems[0] + Nb;
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
                assert(0 <= n && n < N);
                return elems[n / digits<Block>];
        }

        constexpr auto const& block_ref(std::size_t n) const
        {
                assert(0 <= n && n < N);
                return elems[n / digits<Block>];
        }

        // comparators

        constexpr auto do_equal(bit_array const& other) const noexcept
        {
                return elems == other.elems;
        }

        constexpr auto do_colexicographical_compare(bit_array const& other) const noexcept
        {
                for (auto i = Nb - 1; i < Nb; --i) {
                        if (elems[i] < other.elems[i])
                                return true;
                        if (elems[i] > other.elems[i])
                                return false;
                }
                return false;
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
                elems.fill(masks::all<Block>);
        }

        constexpr auto do_reset() noexcept
        {
                elems.fill(masks::none<Block>);
        }

        constexpr auto do_flip() noexcept
        {
                for (auto&& block : elems)
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
                assert(0 <= n && n < N);
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
                for (auto i = n_block - 1; i < Nb; --i)
                        elems[i] = masks::none<Block>;
        }

        constexpr auto do_right_shift(std::size_t n)
        {
                assert(0 <= n && n < N);
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
                for (auto i = Nb - n_block; i < Nb; ++i)
                        elems[i] = masks::none<Block>;
        }

        // observers

        template<std::size_t M>
        constexpr std::enable_if_t<M != 0,
        bool> do_all() const noexcept
        {
                static_assert(0 < M && M < digits<Block>, "");
                for (auto i = 0_z; i < Nb - 1; ++i)
                        if (elems[i] != masks::all<Block>)
                                return false;
                return elems[Nb - 1] == masks::all<Block> >> (digits<Block> - M);
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M == 0,
        bool> do_all() const noexcept
        {
                for (auto&& block : elems)
                        if (block != masks::all<Block>)
                                return false;
                return true;
        }

        constexpr auto do_any() const noexcept
        {
                for (auto&& block : elems)
                        if (block != masks::none<Block>)
                                return true;
                return false;
        }

        constexpr auto do_none() const noexcept
        {
                for (auto&& block : elems)
                        if (block != masks::none<Block>)
                                return false;
                return true;
        }

        constexpr auto do_count() const noexcept
        {
                auto sum = 0_z;
                for (auto&& block : elems)
                        sum += intrinsic::popcount(block);
                return sum;
        }

private:
        // representation

        array<Block, Nb> elems {};
};

}       // namespace xstd