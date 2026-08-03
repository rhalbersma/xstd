//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MEMORY_HPP
#define XSTD_MEMORY_HPP

#include <bit>     // has_single_bit
#include <cassert> // assert
#include <cstddef> // size_t
#include <limits>  // numeric_limits

namespace xstd {

// Rounds size up to the next multiple of alignment, e.g. so a requested
// number of bits can be rounded up to a whole number of storage blocks.
[[nodiscard]] constexpr auto aligned_size(std::size_t alignment, std::size_t size) noexcept
        -> std::size_t
{
        assert(std::has_single_bit(alignment));
        auto const remainder = size % alignment;
        if (remainder == 0) {
                return size;
        }
        auto const padding = alignment - remainder;
        assert(size <= std::numeric_limits<std::size_t>::max() - padding);
        assert((size + padding) % alignment == 0);
        return size + padding;
}

} // namespace xstd

#endif // XSTD_MEMORY_HPP
