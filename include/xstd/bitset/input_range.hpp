#pragma once
#include <xstd/bitset/input_range_facade.hpp>   // InputRangeCoreAcces, InputRangeFacade
#include <xstd/bitset/intrinsic.hpp>            // ctznz
#include <xstd/bitset/masks.hpp>                // none
#include <xstd/limits.hpp>                      // is_unsigned_integer
#include <cassert>                              // assert
#include <cstddef>                              // size_t

namespace xstd {

template<class Block, std::size_t Nb>
class InputRange;

template<class Block>
class InputRange<Block, 1>
:
        // provides member and non-member begin() / end()
        public InputRangeFacade<InputRange<Block, 1>, std::size_t, std::size_t>
{
        static_assert(is_unsigned_integer<Block>, "");

        Block& range;

public:
        explicit InputRange(Block& r) noexcept
        :
                range{r}
        {}

private:
        // gateway for InputRangeFacade to access private implementation
        friend class InputRangeCoreAccess;

        void pop_front()
        {
                assert(!empty());
                range &= range - 1;
        }

        std::size_t front() const
        {
                assert(!empty());
                return intrinsic::ctznz(range);
        }

        bool empty() const noexcept
        {
                return range == masks::none<Block>;
        }
};

}       // namespace xstd
