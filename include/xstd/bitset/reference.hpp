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

public:
        // constructors

        // references cannot be left uninitialized
        ConstReference() = delete;

        constexpr ConstReference(Block const& b, std::size_t n) noexcept
        :
                block_{b},
                index_{n}
        {
                assert(0 <= n & n < N);
        }

        // copying and assignment

        // references can be copied
        constexpr ConstReference(ConstReference const&) = default;

        // references cannot be assigned to
        ConstReference& operator=(ConstReference const&) = delete;

        // const references cannot be assigned through
        ConstReference& operator=(int) = delete;

        // observers

        constexpr ConstIterator<Block, Nb, N> operator&() const noexcept
        {
                return { &block_, index_ };
        }

        /* implicit */ constexpr operator std::size_t() const noexcept
        {
                return index_;
        }

private:
        // representation

        Block const& block_;
        std::size_t index_;
};

}       // namespace xstd