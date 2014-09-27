#pragma once
#include <xstd/bitset/bitset.hpp>       // bitset

namespace xstd {

template<std::size_t N>
constexpr bool set_single(bitset<N> const& b) noexcept
{
        return b.count() == 1;
}

template<std::size_t N>
constexpr bool set_double(bitset<N> const& b) noexcept
{
        return b.count() == 2;
}

template<std::size_t N>
constexpr bool set_multiple(bitset<N> const& b) noexcept
{
        return b.count() >= 2;
}

}       // namespace xstd
