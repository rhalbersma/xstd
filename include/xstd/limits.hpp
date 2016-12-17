#pragma once
#include <cassert>      // assert
#include <cstddef>      // size_t
#include <limits>       // max
#include <type_traits>  // bool_constant

namespace xstd {

template<class T, int N>
constexpr auto is_representable_v = N <= (std::numeric_limits<T>::max() + 1);

template<class T, int N>
using is_representable_t = std::bool_constant<is_representable_v<T, N>>;

template<class T>
constexpr auto is_power_of_2(T x) noexcept
{
        return (x - 1) < (x & - x);
}

constexpr auto align_on(std::size_t offset, std::size_t align) noexcept
{
        assert(is_power_of_2(align));
        return (offset + align - 1) & ~(align - 1);
}

}       // namespace xstd
