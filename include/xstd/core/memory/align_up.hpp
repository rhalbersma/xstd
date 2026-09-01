//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CORE_MEMORY_ALIGN_UP_HPP
#define XSTD_CORE_MEMORY_ALIGN_UP_HPP

#include <xstd/core/concepts/alignable.hpp> // alignable, nothrow_alignable
#include <bit>                              // has_single_bit
#include <cassert>                          // assert
#include <cstddef>                          // size_t

namespace xstd {

// Rounds value up to the next multiple of a power-of-two alignment.
template<alignable T>
[[nodiscard]] constexpr auto align_up(T value, std::size_t alignment) noexcept(nothrow_alignable<T>)
        -> T
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<T>(static_cast<T>(alignment) - static_cast<T>(1));
        auto const sum = static_cast<T>(value + mask);
        // The result is representable, said as the addition not having wrapped, which a modular type may ask after.
        assert(sum >= value);
        return static_cast<T>(sum & static_cast<T>(~mask));
}

} // namespace xstd

#endif // XSTD_CORE_MEMORY_ALIGN_UP_HPP
