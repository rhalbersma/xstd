#pragma once
#include <xstd/bit_array/bit_array_fwd.hpp>     // base_bitset
#include <xstd/bitset/intrinsic.hpp>            // popcount
#include <xstd/bitset/masks.hpp>                // none, one, all
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <cassert>                              // assert
#include <cstddef>                              // size_t
#include <type_traits>                          // enable_if_t
#include <utility>                              // swap

namespace xstd {

template<class Block>
class bit_array<Block, 1>
{
        static_assert(is_unsigned_integer<Block>, "");
        static constexpr auto N = 1 * digits<Block>;

        Block elems {};

protected:
        ~bit_array() = default;

public:
        /* implicit */ constexpr bit_array(Block value) noexcept
        :
                elems{value}
        {}

        constexpr bit_array() = default;
        bit_array(bit_array const&) = default;
        bit_array& operator=(bit_array const&) = default;
        bit_array(bit_array&&) = default;
        bit_array& operator=(bit_array&&) = default;

        // data access

        constexpr auto* block_begin() noexcept
        {
                return &elems;
        }

        constexpr auto const* block_begin() const noexcept
        {
                return &elems;
        }

        constexpr auto* block_end() noexcept
        {
                return &elems;
        }

        constexpr auto const* block_end() const noexcept
        {
                return &elems;
        }

        constexpr auto& block_back() noexcept
        {
                return elems;
        }

        constexpr auto const& block_back() const noexcept
        {
                return elems;
        }

        constexpr auto& block_ref(std::size_t /* n */)
        {
                //assert(n < N);
                return elems;
        }

        constexpr auto const& block_ref(std::size_t /* n */) const
        {
                //assert(n < N);
                return elems;
        }

        // comparators

        constexpr auto do_equal(bit_array const& other) const noexcept
        {
                return elems == other.elems;
        }

        constexpr auto do_colexicographical_compare(bit_array const& other) const noexcept
        {
                return elems < other.elems;
        }

        constexpr auto do_intersects(bit_array const& other) const noexcept
        {
                return (elems & other.elems) != masks::none<Block>;
        }

        constexpr auto do_is_subset_of(bit_array const& other) const noexcept
        {
                return (elems & ~other.elems) == masks::none<Block>;
        }

        constexpr auto do_is_proper_subset_of(bit_array const& other) const noexcept
        {
                if (elems & ~other.elems)
                        return false;
                return (~elems & other.elems) != masks::none<Block>;
        }

        // modifiers

        /* constexpr */ auto do_swap(bit_array& other) noexcept
        {
                using std::swap;
                swap(elems, other.elems);
        }

        constexpr auto do_set() noexcept
        {
                elems = masks::all<Block>;
        }

        constexpr auto do_reset() noexcept
        {
                elems = masks::none<Block>;
        }

        constexpr auto do_flip() noexcept
        {
                elems = ~elems;
        }

        constexpr auto do_and(bit_array const& other) noexcept
        {
                elems &= other.elems;
        }

        constexpr auto do_or(bit_array const& other) noexcept
        {
                elems |= other.elems;
        }

        constexpr auto do_xor(bit_array const& other) noexcept
        {
                elems ^= other.elems;
        }

        constexpr auto do_minus(bit_array const& other) noexcept
        {
                elems &= ~other.elems;
        }

        constexpr auto do_left_shift(std::size_t n)
        {
                assert(n < N);
                elems <<= n;
        }

        constexpr auto do_right_shift(std::size_t n)
        {
                assert(n < N);
                elems >>= n;
        }

        // observers

        template<std::size_t M>
        constexpr std::enable_if_t<M != 0,
        bool> do_all() const noexcept
        {
                static_assert(0 < M && M < digits<Block>, "");
                return elems == masks::all<Block> >> (digits<Block> - M);
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M == 0,
        bool> do_all() const noexcept
        {
                return elems == masks::all<Block>;
        }

        constexpr auto do_any() const noexcept
        {
                return elems != masks::none<Block>;
        }

        constexpr auto do_none() const noexcept
        {
                return elems == masks::none<Block>;
        }

        constexpr auto do_count() const noexcept
        {
                return intrinsic::popcount(elems);
        }
};

}       // namespace xstd
