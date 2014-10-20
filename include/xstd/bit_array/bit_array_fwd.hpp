#pragma once
#include <cstddef>      // size_t

namespace xstd {

template<class Block, std::size_t Nb>
class bit_array;

template<class Block>
class bit_array<Block, 1>;

template<class Block>
class bit_array<Block, 0>;

}       // namespace xstd
