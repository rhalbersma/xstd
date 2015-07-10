#pragma once
#include <xstd/bit/iterator/iterator_fwd.hpp>   // ConstIterator
#include <xstd/bit/iterator/reference_fwd.hpp>  // ConstReference
#include <xstd/bit/primitive.hpp>               // clznz, ctznz
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <boost/iterator/iterator_facade.hpp>   // iterator_core_access, iterator_facade
#include <cassert>                              // assert
#include <cstddef>                              // ptrdiff_t, size_t
#include <iterator>                             // bidirectional_iterator_tag

namespace xstd {
namespace bit {

template<class Block, std::size_t N>
class ConstIterator<Block, 1, N>
:
        public boost::iterator_facade
        <
                ConstIterator<Block, 1, N>,
                std::size_t const,
                std::bidirectional_iterator_tag,
                ConstReference<Block, 1, N>,
                std::ptrdiff_t
        >
{
        static_assert(is_unsigned_integer<Block>, "");
        static_assert(N <= 1 * digits<Block>, "");

        Block const* block{};
        std::size_t index{};

public:
        ConstIterator() = default;

        explicit constexpr ConstIterator(Block const* b)
        :
                block{b},
                index{find_first()}
        {
                assert(block != nullptr);
                assert(index <= N);
        }

        constexpr ConstIterator(Block const* b, std::size_t n)
        :
                block{b},
                index{n}
        {
                assert(block != nullptr);
                assert(index == N);
        }

private:
        // gateway for boost::iterator_facade to access private implementation
        friend class boost::iterator_core_access;

        constexpr auto find_first()
        {
                assert(block != nullptr);
                return *block ? ctznz(*block) : N;
        }

        // operator++() and operator++(int) provided by boost::iterator_facade
        constexpr auto increment()
        {
                assert(block != nullptr);
                assert(index < N);
                if (++index == N)
                        return;
                if (auto const mask = *block >> index)
                        index += ctznz(mask);
                else
                        index = N;
                assert(0 < index && index <= N);
        }

        // operator--() and operator--(int) provided by boost::iterator_facade
        constexpr auto decrement()
        {
                assert(block != nullptr);
                assert(0 < index && index <= N);
                if (--index == 0)
                        return;
                if (auto const mask = *block << (digits<Block> - 1 - index))
                        index -= clznz(mask);
                else
                        index = 0;
                assert(index < N);
        }

        // operator* provided by boost::iterator_facade
        constexpr ConstReference<Block, 1, N> dereference() const
        {
                assert(block != nullptr);
                assert(index < N);
                return { *block, index };
        }

        // operator== and operator!= provided by boost::iterator_facade
        constexpr auto equal(ConstIterator const& other) const noexcept
        {
                return block == other.block && index == other.index;
        }
};

}       // namespace bit
}       // namespace xstd
