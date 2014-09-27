#pragma once
#include <xstd/bitset/detail/base_bitset_fwd.hpp>       // base_bitset
#include <xstd/bitset/intrinsic.hpp>                    // popcount
#include <xstd/bitset/limits.hpp>                       // digits, is_unsigned_integer
#include <xstd/bitset/masks.hpp>                        // none, one, all
#include <cassert>                                      // assert
#include <cstddef>                                      // size_t
#include <type_traits>                                  // enable_if_t
#include <utility>                                      // swap

namespace xstd {
namespace detail {

template<class Block>
class base_bitset<Block, 1>
{
        static_assert(is_unsigned_integer<Block>, "");
        static constexpr auto N = 1 * digits<Block>;
public:
        // constructors

        constexpr base_bitset() = default;

        /* implicit */ constexpr base_bitset(Block value) noexcept
        :
                elems{value}
        {}

protected:
        // destructor

        ~base_bitset() = default;

public:
        // copying, moving and assignment

        base_bitset(base_bitset const&) = default;
        base_bitset(base_bitset&&) = default;
        base_bitset& operator=(base_bitset const&) = default;
        base_bitset& operator=(base_bitset&&) = default;

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
                //assert(0 <= n && n < N);
                return elems;
        }

        constexpr auto const& block_ref(std::size_t /* n */) const
        {
                //assert(0 <= n && n < N);
                return elems;
        }

        // comparators

        constexpr auto do_equal(base_bitset const& other) const noexcept
        {
                return elems == other.elems;
        }

        constexpr auto do_colexicographical_compare(base_bitset const& other) const noexcept
        {
                return elems < other.elems;
        }

        constexpr auto do_intersects(base_bitset const& other) const noexcept
        {
                return (elems & other.elems) != masks::none<Block>;
        }

        constexpr auto do_is_subset_of(base_bitset const& other) const noexcept
        {
                return (elems & ~other.elems) == masks::none<Block>;
        }

        constexpr auto do_is_proper_subset_of(base_bitset const& other) const noexcept
        {
                if (elems & ~other.elems)
                        return false;
                return (~elems & other.elems) != masks::none<Block>;
        }

        // modifiers

        /* constexpr */ auto do_swap(base_bitset& other) noexcept
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

        constexpr auto do_and(base_bitset const& other) noexcept
        {
                elems &= other.elems;
        }

        constexpr auto do_or(base_bitset const& other) noexcept
        {
                elems |= other.elems;
        }

        constexpr auto do_xor(base_bitset const& other) noexcept
        {
                elems ^= other.elems;
        }

        constexpr auto do_minus(base_bitset const& other) noexcept
        {
                elems &= ~other.elems;
        }

        constexpr auto do_left_shift(std::size_t n)
        {
                assert(0 <= n && n < N);
                elems <<= n;
        }

        constexpr auto do_right_shift(std::size_t n)
        {
                assert(0 <= n && n < N);
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

private:
        // representation

        Block elems{};
};

}       // namespace detail
}       // namespace xstd
