#pragma once
#include <xstd/bitset/input_range_facade.hpp>   // InputRangeCoreAcces, InputRangeFacade
#include <xstd/bitset/intrinsic.hpp>            // ctznz
#include <xstd/bitset/masks.hpp>                // none
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

        Block& range_;

public:
        explicit InputRange(Block& range) noexcept
        :
                range_{range}
        {}

private:
        // gateway for InputRangeFacade to access private implementation
        friend class InputRangeCoreAccess;

        void pop_front()
        {
                assert(!empty());
                range_ &= range_ - 1;
        }

        std::size_t front() const
        {
                assert(!empty());
                return intrinsic::ctznz(range_);
        }

        bool empty() const noexcept
        {
                return range_ == masks::none<Block>;
        }
};

}       // namespace xstd
