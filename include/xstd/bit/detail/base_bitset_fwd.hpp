#pragma once

namespace xstd {
namespace bit {
namespace detail {

template<class Block, int Nb>
struct base_bitset;

template<class Block>
struct base_bitset<Block, 1>;

template<class Block>
struct base_bitset<Block, 0>;

}       // namespace detail
}       // namespace bit
}       // namespace xstd
