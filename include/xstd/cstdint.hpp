#pragma once
#include <xstd/limits.hpp>      // is_representable
#include <cstddef>              // size_t
#include <cstdint>              // uint8_t, uint16_t, uint32_t, uint64_t
                                // uint_fast8_t, uint_fast16_t, uint_fast32_t, uint_fast64_t
                                // uint_least8_t, uint_least16_t, uint_least32_t, uint_least64_t
#include <type_traits>          // enable_if

namespace xstd {
namespace detail {

template<std::size_t, class = void>
struct uint;

template<std::size_t N>
struct uint<N, std::enable_if_t<is_representable_v<uint8_t, N>>>
{
        using type = uint8_t;
};

template<std::size_t N>
struct uint<N, std::enable_if_t<is_representable_v<uint16_t, N> && !is_representable_v<uint8_t, N>>>
{
        using type = uint16_t;
};

template<std::size_t N>
struct uint<N, std::enable_if_t<is_representable_v<uint32_t, N> && !is_representable_v<uint16_t, N>>>
{
        using type = uint32_t;
};

template<std::size_t N>
struct uint<N, std::enable_if_t<is_representable_v<uint64_t, N> && !is_representable_v<uint32_t, N>>>
{
        using type = uint64_t;
};

template<std::size_t, class = void>
struct uint_fast;

template<std::size_t N>
struct uint_fast<N, std::enable_if_t<is_representable_v<uint8_t, N>>>
{
        using type = uint_fast8_t;
};

template<std::size_t N>
struct uint_fast<N, std::enable_if_t<is_representable_v<uint16_t, N> && !is_representable_v<uint8_t, N>>>
{
        using type = uint_fast16_t;
};

template<std::size_t N>
struct uint_fast<N, std::enable_if_t<is_representable_v<uint32_t, N> && !is_representable_v<uint16_t, N>>>
{
        using type = uint_fast32_t;
};

template<std::size_t N>
struct uint_fast<N, std::enable_if_t<is_representable_v<uint64_t, N> && !is_representable_v<uint32_t, N>>>
{
        using type = uint_fast64_t;
};

template<std::size_t, class = void>
struct uint_least;

template<std::size_t N>
struct uint_least<N, std::enable_if_t<is_representable_v<uint8_t, N>>>
{
        using type = uint_least8_t;
};

template<std::size_t N>
struct uint_least<N, std::enable_if_t<is_representable_v<uint16_t, N> && !is_representable_v<uint8_t, N>>>
{
        using type = uint_least16_t;
};

template<std::size_t N>
struct uint_least<N, std::enable_if_t<is_representable_v<uint32_t, N> && !is_representable_v<uint16_t, N>>>
{
        using type = uint_least32_t;
};

template<std::size_t N>
struct uint_least<N, std::enable_if_t<is_representable_v<uint64_t, N> && !is_representable_v<uint32_t, N>>>
{
        using type = uint_least64_t;
};

}       // namespace detail

template<std::size_t N>
using uint_t = typename detail::uint<N>::type;

template<std::size_t N>
using uint_fast_t = typename detail::uint_fast<N>::type;

template<std::size_t N>
using uint_least_t = typename detail::uint_least<N>::type;

}       // namespace xstd
