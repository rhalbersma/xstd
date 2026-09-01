//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_MEMORY_ALIGN_UP_HPP
#define XSTD_INTS_MEMORY_ALIGN_UP_HPP

#include <xstd/ints/concepts/alignable.hpp> // alignable, nothrow_alignable
#include <bit>                              // has_single_bit
#include <cassert>                          // assert
#include <cstddef>                          // size_t
#include <cstdint>                          // uintptr_t

namespace xstd {

// Rounds value up to the next multiple of a power-of-two alignment.
template<alignable T>
[[nodiscard]] constexpr auto align_up(T value, std::size_t alignment) noexcept(nothrow_alignable<T>)
        -> T
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<T>(alignment - 1);
        auto const sum = static_cast<T>(value + mask);
        // The result is representable, said as the addition not having wrapped, which a modular type may ask after.
        assert(sum >= value);
        // Subtracting the remainder is what alignment is; & mask is only how a radix-2 type spells % alignment.
        return static_cast<T>(sum - static_cast<T>(sum & mask));
}

// The same arithmetic in the address space, which is why it delegates rather than repeating
// the mask work. Qualified, as the cstdlib family is: uintptr_t brings no associated namespace
// along, so nothing can be found here today, but the delegation says which overload it means
// rather than leaving that to a lookup that a later overload or argument type could redirect.
template<class T>
[[nodiscard]] auto align_up(T* pointer, std::size_t alignment) noexcept
        -> T*
{
        // NOLINTNEXTLINE(performance-no-int-to-ptr): the address is what was aligned, and it has to become a pointer again
        return reinterpret_cast<T*>(xstd::align_up(reinterpret_cast<std::uintptr_t>(pointer), alignment));
}

} // namespace xstd

#endif // XSTD_INTS_MEMORY_ALIGN_UP_HPP
