#pragma once
#include <xstd/bit/input_range_facade.hpp>      // InputRangeCoreAcces, InputRangeFacade
#include <xstd/bit/mask.hpp>                    // none
#include <xstd/bit/primitive.hpp>               // ctznz
#include <xstd/limits.hpp>                      // is_unsigned_integer
#include <cassert>                              // assert
#include <cstddef>                              // size_t

namespace xstd {
namespace bit {

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
        friend class xstd::InputRangeCoreAccess;

        auto pop_front()
        {
                assert(!empty());
                range &= range - 1;
        }

        auto front() const
        {
                assert(!empty());
                return ctznz(range);
        }

        auto empty() const noexcept
        {
                return range == mask::none<Block>;
        }
};

}       // namespace bit
}       // namespace xstd
