#pragma once
#include <xstd/bitset/limits.hpp>       // digits
#include <xstd/bitset/masks.hpp>        // one
#include <cassert>                      // assert
#include <cstddef>                      // size_t

namespace xstd {
namespace loop {

template<class T>
constexpr auto ctz(T x) noexcept
{
        for (std::size_t i = 0; i < digits<T>; ++i)
                if (x & (masks::one<T> << i))
                        return i;
        return digits<T>;
}

template<class T>
constexpr auto clz(T x) noexcept
{
        for (auto i = digits<T> - 1; i < digits<T>; --i)
                if (x & (masks::one<T> << i))
                        return digits<T> - 1 - i;
        return digits<T>;
}

template<class T>
constexpr auto ctznz(T x) noexcept
{
        return ctz(x);
}

template<class T>
constexpr auto clznz(T x) noexcept
{
        return clz(x);
}

template<class T>
constexpr auto bsfnz(T x) noexcept
{
        return ctznz(x);
}

template<class T>
constexpr auto bsrnz(T x) noexcept
{
        return digits<T> - 1 - clznz(x);
}

template<class T>
constexpr auto popcount(T x) noexcept
{
        std::size_t n = 0;
        for (; x; x &= x - masks::one<T>)
                ++n;
        assert(0 <= n && n <= digits<T>);
        return n;
}

}       // namespace loop
}       // namespace xstd
