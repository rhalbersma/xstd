#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>      // uint8_t, uint16_t, uint32_t, uint64_t
                        // uint_fast8_t, uint_fast16_t, uint_fast32_t, uint_fast64_t
                        // uint_least8_t, uint_least16_t, uint_least32_t, uint_least64_t
#include <limits>       // min, max
#include <type_traits>  // conditional_t

namespace xstd {

template<class T, int N>
constexpr auto is_representable_v = std::numeric_limits<T>::min() <= N && N <= std::numeric_limits<T>::max();

template<int N>
using uint_t = 
        std::conditional_t<is_representable_v<uint8_t, N>, uint8_t, 
        std::conditional_t<is_representable_v<uint16_t, N>, uint16_t, 
        std::conditional_t<is_representable_v<uint32_t, N>, uint32_t, 
        uint64_t
>>>;

template<int N>
using uint_fast_t = 
        std::conditional_t<is_representable_v<uint8_t, N>, uint_fast8_t, 
        std::conditional_t<is_representable_v<uint16_t, N>, uint_fast16_t, 
        std::conditional_t<is_representable_v<uint32_t, N>, uint_fast32_t, 
        uint_fast64_t
>>>;

template<int N>
using uint_least_t = 
        std::conditional_t<is_representable_v<uint8_t, N>, uint_least8_t, 
        std::conditional_t<is_representable_v<uint16_t, N>, uint_least16_t, 
        std::conditional_t<is_representable_v<uint32_t, N>, uint_least32_t, 
        uint_least64_t
>>>;

}       // namespace xstd
