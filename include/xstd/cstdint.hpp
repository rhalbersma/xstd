#include <xstd/limits.hpp>      // is_representable
#include <cstddef>              // size_t
#include <cstdint>              // uint_fast8_t, uint_fast16_t, uint_fast32_t, uint_fast64_t
                                // uint_least8_t, uint_least16_t, uint_least32_t, uint_least64_t
#include <type_traits>          // enable_if

namespace xstd {

template<std::size_t, class = void>
struct uint_fast;

template<std::size_t N>
struct uint_fast<N,
        std::enable_if_t<is_representable<uint_fast8_t>(N)>>
{
        using type = uint_fast8_t;
};

template<std::size_t N>
struct uint_fast<N,
        std::enable_if_t<is_representable<uint_fast16_t>(N) && !is_representable<uint_fast8_t>(N)>>
{
        using type = uint_fast16_t;
};

template<std::size_t N>
struct uint_fast<N,
        std::enable_if_t<is_representable<uint_fast32_t>(N) && !is_representable<uint_fast16_t>(N)>>
{
        using type = uint_fast32_t;
};

template<std::size_t N>
struct uint_fast<N,
        std::enable_if_t<!is_representable<uint_fast32_t>(N)>>
{
        using type = uint_fast64_t;
};

template<std::size_t N>
using uint_fast_t = typename uint_fast<N>::type;

template<std::size_t, class = void>
struct uint_least;

template<std::size_t N>
struct uint_least<N,
        std::enable_if_t<is_representable<uint_least8_t>(N)>>
{
        using type = uint_least8_t;
};

template<std::size_t N>
struct uint_least<N,
        std::enable_if_t<is_representable<uint_least16_t>(N) && !is_representable<uint_least8_t>(N)>>
{
        using type = uint_least16_t;
};

template<std::size_t N>
struct uint_least<N,
        std::enable_if_t<is_representable<uint_least32_t>(N) && !is_representable<uint_least16_t>(N)>>
{
        using type = uint_least32_t;
};

template<std::size_t N>
struct uint_least<N,
        std::enable_if_t<!is_representable<uint_least32_t>(N)>>
{
        using type = uint_least64_t;
};

template<std::size_t N>
using uint_least_t = typename uint_least<N>::type;

}       // namespace xstd
