#pragma once
#include <xstd/bit_array/bit_array_fwd.hpp>     // bit_array (primary template)
#include <xstd/limits.hpp>                      // is_unsigned_integer
#include <cstddef>                              // size_t

namespace xstd {
namespace bit {

template<class Block>
struct bit_array<Block, 0>
{
        static_assert(is_unsigned_integer<Block>, "");

        bit_array() = default;

        // data access

        constexpr auto*       block_begin()       noexcept { return this; }
        constexpr auto const* block_begin() const noexcept { return this; }
        constexpr auto*       block_end  ()       noexcept { return this; }
        constexpr auto const* block_end  () const noexcept { return this; }

        constexpr auto&       block_back()                  noexcept { return *static_cast<Block*      >(nullptr); }
        constexpr auto const& block_back()            const noexcept { return *static_cast<Block const*>(nullptr); }
        constexpr auto&       block_ref (std::size_t)       noexcept { return *static_cast<Block*      >(nullptr); }
        constexpr auto const& block_ref (std::size_t) const noexcept { return *static_cast<Block const*>(nullptr); }

        // comparators

        constexpr auto do_equal                    (bit_array const&) const noexcept { return true;  }
        constexpr auto do_colexicographical_compare(bit_array const&) const noexcept { return false; }
        constexpr auto do_intersects               (bit_array const&) const noexcept { return false; }
        constexpr auto do_is_subset_of             (bit_array const&) const noexcept { return true;  }
        constexpr auto do_is_proper_subset_of      (bit_array const&) const noexcept { return false; }

        // modifiers

        constexpr auto do_swap(bit_array&) noexcept {}

        constexpr auto do_set  () noexcept {}
        constexpr auto do_reset() noexcept {}
        constexpr auto do_flip () noexcept {}

        constexpr auto do_and  (bit_array const&) noexcept {}
        constexpr auto do_or   (bit_array const&) noexcept {}
        constexpr auto do_xor  (bit_array const&) noexcept {}
        constexpr auto do_minus(bit_array const&) noexcept {}

        constexpr auto do_left_shift (std::size_t) {}
        constexpr auto do_right_shift(std::size_t) {}

        // observers

        template<std::size_t M>
        constexpr auto        do_all  () const noexcept { static_assert(M == 0, ""); return true; }

        constexpr auto        do_any  () const noexcept { return false; }
        constexpr auto        do_none () const noexcept { return true;  }
        constexpr std::size_t do_count() const noexcept { return 0;     }
};

}       // namespace bit
}       // namespace xstd
