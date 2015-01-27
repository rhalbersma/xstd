#pragma once
#include <xstd/bitset/iterator/iterator_fwd.hpp>        // ConstIterator
#include <xstd/bitset/iterator/reference_fwd.hpp>       // ConstReference
#include <xstd/bitset/limits.hpp>                       // digits, is_unsigned_integer
#include <cassert>                                      // assert
#include <cstddef>                                      // size_t

namespace xstd {

template<class Block, std::size_t Nb, std::size_t N>
class ConstReference
{
        static_assert(is_unsigned_integer<Block>, "");
        static_assert(N <= Nb * digits<Block>, "");

        Block const& block;
        std::size_t index;

public:
        constexpr ConstReference(Block const& b, std::size_t n) noexcept
        :
                block{b},
                index{n}
        {
                assert(n < N);
        }

        /* implicit */ constexpr operator std::size_t() const noexcept
        {
                return index;
        }

        // references cannot be left uninitialized
        constexpr ConstReference() = delete;

        // references can be copied
        constexpr ConstReference(ConstReference const&) = default;

        // references cannot be assigned to
        constexpr ConstReference& operator=(ConstReference const&) = delete;

        // const references cannot be assigned through
        constexpr ConstReference& operator=(std::size_t) = delete;

        constexpr ConstIterator<Block, Nb, N> operator&() const noexcept
        {
                return { &block, index };
        }
};

}       // namespace xstd
