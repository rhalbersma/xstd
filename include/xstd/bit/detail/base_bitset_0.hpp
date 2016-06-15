#pragma once
#include <xstd/bit/detail/base_bitset_fwd.hpp>  // base_bitset
#include <xstd/limits.hpp>                      // is_unsigned_integer
#include <cstddef>                              // size_t
#include <utility>                              // move

namespace xstd {
namespace bit {
namespace detail {

template<class Block>
struct base_bitset<Block, 0>
{
        static_assert(is_unsigned_integer<Block>);

        base_bitset() = default;

        // data access

        constexpr auto*       block_begin()       noexcept { return this; }
        constexpr auto const* block_begin() const noexcept { return this; }
        constexpr auto*       block_end  ()       noexcept { return this; }
        constexpr auto const* block_end  () const noexcept { return this; }

        constexpr auto&       block_back()                  noexcept { return *static_cast<Block*      >(nullptr); }
        constexpr auto const& block_back()            const noexcept { return *static_cast<Block const*>(nullptr); }
        constexpr auto&       block_ref (std::size_t)       noexcept { return *static_cast<Block*      >(nullptr); }
        constexpr auto const& block_ref (std::size_t) const noexcept { return *static_cast<Block const*>(nullptr); }

        constexpr auto        block_mask(std::size_t) const noexcept { return Block{}; }

        // comparators

        constexpr auto op_equal_to    (base_bitset const&) const noexcept { return true;  }
        constexpr auto op_less        (base_bitset const&) const noexcept { return false; }
        constexpr auto do_intersects  (base_bitset const&) const noexcept { return false; }
        constexpr auto do_is_subset_of(base_bitset const&) const noexcept { return true;  }

        // modifiers

        constexpr auto do_swap(base_bitset&) noexcept {}

        constexpr auto do_set  () noexcept {}
        constexpr auto do_reset() noexcept {}
        constexpr auto op_flip () noexcept {}

        constexpr auto op_and  (base_bitset const&) noexcept {}
        constexpr auto op_or   (base_bitset const&) noexcept {}
        constexpr auto op_xor  (base_bitset const&) noexcept {}
        constexpr auto op_minus(base_bitset const&) noexcept {}

        constexpr auto op_left_shift (std::size_t const) {}
        constexpr auto op_right_shift(std::size_t const) {}

        template<class UnaryFunction>
        constexpr auto do_for_each(UnaryFunction f) const { return std::move(f); }

        template<class UnaryFunction>
        constexpr auto do_reverse_for_each(UnaryFunction f) const { return std::move(f); }

        // observers

        template<std::size_t M>
        constexpr auto do_all  () const noexcept { static_assert(M == 0); return true; }

        constexpr auto do_any  () const noexcept { return false; }
        constexpr auto do_none () const noexcept { return true;  }
        constexpr auto do_count() const noexcept { return std::size_t{0}; }
};

}       // namespace detail
}       // namespace bit
}       // namespace xstd
