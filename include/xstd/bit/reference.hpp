#pragma once
#include <xstd/bit/iterator/iterator_fwd.hpp>   // ConstIterator
#include <xstd/bit/iterator/reference_fwd.hpp>  // ConstReference
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <cassert>                              // assert
#include <cstddef>                              // size_t

namespace xstd {
namespace bit {

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

        ConstReference() = delete;
        ConstReference(ConstReference const&) = default;
        ConstReference& operator=(ConstReference const&) = delete;
        ConstReference& operator=(std::size_t) = delete;

        constexpr ConstIterator<Block, Nb, N> operator&() const noexcept
        {
                return { &block, index };
        }
};

}       // namespace bit
}       // namespace xstd
