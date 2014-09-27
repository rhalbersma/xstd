#pragma once
#include <cstddef>      // size_t

namespace xstd {

template<class Block, std::size_t Nb, std::size_t N>
class ConstIterator;

template<class Block, std::size_t N>
class ConstIterator<Block, 1, N>;

}       // namespace xstd
