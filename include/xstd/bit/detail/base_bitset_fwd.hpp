#pragma once
#include <cstddef>      // size_t

namespace xstd {
namespace bit {
namespace detail {

template<class Block, std::size_t Nb>
struct base_bitset;

template<class Block>
struct base_bitset<Block, 1>;

template<class Block>
struct base_bitset<Block, 0>;

}       // namespace detail
}       // namespace bit
}       // namespace xstd
