//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CORE_MEMORY_ALIGN_DOWN_HPP
#define XSTD_CORE_MEMORY_ALIGN_DOWN_HPP

#include <xstd/core/concepts/alignable.hpp> // alignable, nothrow_alignable
#include <bit>                              // has_single_bit
#include <cassert>                          // assert
#include <cstddef>                          // size_t

namespace xstd {

// Rounds value down to the previous multiple of a power-of-two alignment.
template<alignable T>
[[nodiscard]] constexpr auto align_down(T value, std::size_t alignment) noexcept(nothrow_alignable<T>)
        -> T
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<T>(static_cast<T>(alignment) - static_cast<T>(1));
        return static_cast<T>(value & static_cast<T>(~mask));
}

} // namespace xstd

#endif // XSTD_CORE_MEMORY_ALIGN_DOWN_HPP
