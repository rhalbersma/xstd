#pragma once
#include <cstddef>      // size_t

namespace xstd {
namespace bit {

template<class Block, std::size_t Nb>
struct bit_array;

template<class Block>
struct bit_array<Block, 1>;

template<class Block>
struct bit_array<Block, 0>;

}       // namespace bit
}       // namespace xstd
