#pragma once
#include <cstddef>      // size_t

namespace xstd {
namespace detail {

template<class Block, std::size_t Nb>
class base_bitset;

template<class Block>
class base_bitset<Block, 1>;

template<class Block>
class base_bitset<Block, 0>;

}       // namespace detail
}       // namespace xstd
