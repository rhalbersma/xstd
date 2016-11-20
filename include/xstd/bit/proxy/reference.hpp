#pragma once
#include <xstd/bit/proxy/reference_fwd.hpp>     // ConstReference
#include <xstd/bit/proxy/iterator_fwd.hpp>      // ConstIterator
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <cassert>                              // assert

namespace xstd {
namespace bit {

template<class Block, int Nb, int N>
class ConstReference
{
        static_assert(is_unsigned_integer<Block>);
        static_assert(N <= Nb * digits<Block>);

        Block const& block;
        int index;

public:
        ConstReference() = delete;
        ConstReference(ConstReference const&) = default;
        ConstReference& operator=(ConstReference const&) = delete;
        ConstReference& operator=(int) = delete;

        constexpr ConstReference(Block const& b, int n) noexcept
        :
                block{b},
                index{n}
        {
                assert(n < N);
        }

        /* implicit */ constexpr operator int() const noexcept
        {
                return index;
        }

        constexpr ConstIterator<Block, Nb, N> operator&() const noexcept
        {
                return { &block, index };
        }
};

}       // namespace bit
}       // namespace xstd
