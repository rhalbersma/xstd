#pragma once
#include <xstd/bit/proxy/iterator_fwd.hpp>      // ConstIterator
#include <xstd/bit/proxy/reference_fwd.hpp>     // ConstReference
#include <xstd/bit/primitive.hpp>               // bsfnz, bsrnz, clznz, ctznz
#include <xstd/cstddef.hpp>                     // _z
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <boost/iterator/iterator_facade.hpp>   // iterator_core_access, iterator_facade
#include <cassert>                              // assert
#include <cstddef>                              // ptrdiff_t
#include <iterator>                             // bidirectional_iterator_tag

namespace xstd {
namespace bit {

template<class Block, std::size_t Nb, std::size_t N>
class ConstIterator
:
        public boost::iterator_facade
        <
                ConstIterator<Block, Nb, N>,
                std::size_t const,
                std::bidirectional_iterator_tag,
                ConstReference<Block, Nb, N>,
                std::ptrdiff_t
        >
{
        static_assert(is_unsigned_integer<Block>);
        static_assert(N <= Nb * digits<Block>);

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
                for (auto i = 0_z; i < Nb; ++i) {
                        if (auto const mask = *block) {
                                assert(i * digits<Block> + bsfnz(mask) < N);
                                return i * digits<Block> + bsfnz(mask);
                        }
                        ++block;
                }
                return N;
        }

        // operator++() and operator++(int) provided by boost::iterator_facade
        constexpr auto increment()
        {
                assert(block != nullptr);
                assert(index < N);
                if (++index == N) {
                        ++block;
                        return;
                }

                auto const idx = index % digits<Block>;
                if (idx == 0)
                        ++block;
                if (auto const mask = *block >> idx) {
                        index += ctznz(mask);
                        assert(index < N);
                        return;
                }
                ++block;

                for (auto i = index / digits<Block> + 1; i < Nb; ++i) {
                        if (auto const mask = *block) {
                                index = i * digits<Block> + bsfnz(mask);
                                assert(index < N);
                                return;
                        }
                        ++block;
                }
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

                auto const idx = index % digits<Block>;
                if (idx == digits<Block> - 1 || index == N - 1)
                        --block;
                if (auto const mask = *block << (digits<Block> - 1 - idx)) {
                        index -= clznz(mask);
                        assert(index < N);
                        return;
                }
                --block;

                for (auto i = index / digits<Block> - 1; i < Nb; --i) {
                        if (auto const mask = *block) {
                                index = i * digits<Block> + bsrnz(mask);
                                assert(index < N);
                                return;
                        }
                        --block;
                }
                index = 0;
                assert(index < N);
        }

        // operator* provided by boost::iterator_facade
        constexpr ConstReference<Block, Nb, N> dereference() const
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
