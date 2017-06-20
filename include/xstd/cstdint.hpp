#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>      // uint8_t, uint16_t, uint32_t, uint64_t
                        // uint_fast8_t, uint_fast16_t, uint_fast32_t, uint_fast64_t
                        // uint_least8_t, uint_least16_t, uint_least32_t, uint_least64_t
#include <limits>       // max
#include <type_traits>  // enable_if, bool_constant

namespace xstd {
namespace detail {

template<class T, int N>
constexpr auto is_representable_v = N <= (std::numeric_limits<T>::max() + 1);

template<int, class = void>
struct uint;

template<int N>
struct uint<N, std::enable_if_t<
        is_representable_v<uint8_t, N>
>>
{
        using type = uint8_t;
};

template<int N>
struct uint<N, std::enable_if_t<
        is_representable_v<uint16_t, N> && not is_representable_v<uint8_t, N>
>>
{
        using type = uint16_t;
};

template<int N>
struct uint<N, std::enable_if_t<
        is_representable_v<uint32_t, N> && not is_representable_v<uint16_t, N>
>>
{
        using type = uint32_t;
};

template<int N>
struct uint<N, std::enable_if_t<
        is_representable_v<uint64_t, N> && not is_representable_v<uint32_t, N>
>>
{
        using type = uint64_t;
};

template<int, class = void>
struct uint_fast;

template<int N>
struct uint_fast<N, std::enable_if_t<
        is_representable_v<uint8_t, N>
>>
{
        using type = uint_fast8_t;
};

template<int N>
struct uint_fast<N, std::enable_if_t<
        is_representable_v<uint16_t, N> && not is_representable_v<uint8_t, N>
>>
{
        using type = uint_fast16_t;
};

template<int N>
struct uint_fast<N, std::enable_if_t<
        is_representable_v<uint32_t, N> && not is_representable_v<uint16_t, N>
>>
{
        using type = uint_fast32_t;
};

template<int N>
struct uint_fast<N, std::enable_if_t<
        is_representable_v<uint64_t, N> && not is_representable_v<uint32_t, N>
>>
{
        using type = uint_fast64_t;
};

template<int, class = void>
struct uint_least;

template<int N>
struct uint_least<N, std::enable_if_t<
        is_representable_v<uint8_t, N>
>>
{
        using type = uint_least8_t;
};

template<int N>
struct uint_least<N, std::enable_if_t<
        is_representable_v<uint16_t, N> && not is_representable_v<uint8_t, N>
>>
{
        using type = uint_least16_t;
};

template<int N>
struct uint_least<N, std::enable_if_t<
        is_representable_v<uint32_t, N> && not is_representable_v<uint16_t, N>
>>
{
        using type = uint_least32_t;
};

template<int N>
struct uint_least<N, std::enable_if_t<
        is_representable_v<uint64_t, N> && not is_representable_v<uint32_t, N>
>>
{
        using type = uint_least64_t;
};

}       // namespace detail

template<int N>
using uint_t = typename detail::uint<N>::type;

template<int N>
using uint_fast_t = typename detail::uint_fast<N>::type;

template<int N>
using uint_least_t = typename detail::uint_least<N>::type;

}       // namespace xstd
