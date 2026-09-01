//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MEMORY_ALIGN_DOWN_HPP
#define XSTD_MEMORY_ALIGN_DOWN_HPP

#include <xstd/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <bit>                                       // has_single_bit
#include <cassert>                                   // assert
#include <cstddef>                                   // size_t

namespace xstd {

// Rounds value down to the previous multiple of a power-of-two alignment.
template<unsigned_integer I>
[[nodiscard]] constexpr auto align_down(I value, std::size_t alignment) noexcept(nothrow_const_operators<I>)
        -> I
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<I>(static_cast<I>(alignment) - static_cast<I>(1));
        return static_cast<I>(value & static_cast<I>(~mask));
}

} // namespace xstd

#endif // XSTD_MEMORY_ALIGN_DOWN_HPP
