//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MEMORY_ALIGN_UP_HPP
#define XSTD_MEMORY_ALIGN_UP_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // has_single_bit
#include <cassert>                                        // assert
#include <cstddef>                                        // size_t

namespace xstd {

// Rounds value up to the next multiple of a power-of-two alignment.
template<unsigned_integer I>
[[nodiscard]] constexpr auto align_up(I value, std::size_t alignment) noexcept(nothrow_const_operators<I>)
        -> I
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<I>(static_cast<I>(alignment) - static_cast<I>(1));
        assert(value <= static_cast<I>(numeric_limits<I>::max() - mask));
        return static_cast<I>(static_cast<I>(value + mask) & static_cast<I>(~mask));
}

} // namespace xstd

#endif // XSTD_MEMORY_ALIGN_UP_HPP
