#pragma once
#include <xstd/bit/detail/base_bitset_fwd.hpp>  // base_bitset
#include <xstd/bit/mask.hpp>                    // none, one, all
#include <xstd/bit/primitive.hpp>               // ctznz, popcount
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <cassert>                              // assert
#include <cstddef>                              // size_t
#include <type_traits>                          // enable_if
#include <utility>                              // swap

namespace xstd {
namespace bit {
namespace detail {

template<class Block>
struct base_bitset<Block, 1>
{
        static_assert(is_unsigned_integer<Block>);
        static constexpr auto N = 1 * digits<Block>;

        Block elems;

        base_bitset() = default;

        /* implicit */ constexpr base_bitset(Block value) noexcept
        :
                elems{value}
        {}

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

        constexpr auto& block_ref(std::size_t)
        {
                return elems;
        }

        constexpr auto const& block_ref(std::size_t) const
        {
                return elems;
        }

        constexpr auto block_mask(std::size_t n) const noexcept
        {
                assert(n < N);
                return mask::one<Block> << n;
        }

        // comparators

        constexpr auto op_equal_to(base_bitset const& other) const noexcept
        {
                return elems == other.elems;
        }

        constexpr auto op_less(base_bitset const& other) const noexcept
        {
                return elems < other.elems;
        }

        constexpr auto do_intersects(base_bitset const& other) const noexcept
        {
                return elems & other.elems;
        }

        constexpr auto do_is_subset_of(base_bitset const& other) const noexcept
        {
                return !(elems & ~other.elems);
        }

        // modifiers

        auto do_swap(base_bitset& other) noexcept
        {
                using std::swap;
                swap(elems, other.elems);
        }

        constexpr auto do_set() noexcept
        {
                elems = mask::all<Block>;
        }

        constexpr auto do_reset() noexcept
        {
                elems = mask::none<Block>;
        }

        constexpr auto op_flip() noexcept
        {
                elems = ~elems;
        }

        constexpr auto op_and(base_bitset const& other) noexcept
        {
                elems &= other.elems;
        }

        constexpr auto op_or(base_bitset const& other) noexcept
        {
                elems |= other.elems;
        }

        constexpr auto op_xor(base_bitset const& other) noexcept
        {
                elems ^= other.elems;
        }

        constexpr auto op_minus(base_bitset const& other) noexcept
        {
                elems &= ~other.elems;
        }

        constexpr auto op_left_shift(std::size_t const n)
        {
                assert(n < N);
                elems <<= n;
        }

        constexpr auto op_right_shift(std::size_t const n)
        {
                assert(n < N);
                elems >>= n;
        }

        template<class UnaryFunction>
        constexpr auto do_for_each(UnaryFunction f) const
        {
                for (auto block = elems; block;) {
                        auto const first = bsfnz(block);
                        f(first);
                        block ^= block_mask(first);
                }
                return std::move(f);
        }

        template<class UnaryFunction>
        constexpr auto do_reverse_for_each(UnaryFunction f) const
        {
                for (auto block = elems; block;) {
                        auto const last = bsrnz(block);
                        f(last);
                        block ^= block_mask(last);
                }
                return std::move(f);
        }

        // observers

        template<std::size_t M, std::enable_if_t<M != 0>* = nullptr>
        constexpr auto do_all() const noexcept
        {
                static_assert(M < digits<Block>);
                return elems == mask::all<Block> >> (digits<Block> - M);
        }

        template<std::size_t M, std::enable_if_t<M == 0>* = nullptr>
        constexpr auto do_all() const noexcept
        {
                return elems == mask::all<Block>;
        }

        constexpr auto do_any() const noexcept
        {
                return elems != mask::none<Block>;
        }

        constexpr auto do_none() const noexcept
        {
                return elems == mask::none<Block>;
        }

        constexpr auto do_count() const noexcept
        {
                return popcount(elems);
        }
};

}       // namespace detail
}       // namespace bit
}       // namespace xstd
