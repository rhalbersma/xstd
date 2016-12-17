#pragma once
#include <cassert>      // assert
#include <limits>       // digits

namespace xstd {
namespace bit {
namespace loop {

template<class T>
constexpr auto ctznz(T x) noexcept
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                if (x & (static_cast<T>(1) << i)) {
                        return i;
                }
        }
        return std::numeric_limits<T>::digits;
}

template<class T>
constexpr auto clznz(T x) noexcept
{
        for (auto i = std::numeric_limits<T>::digits - 1; i >= 0; --i) {
                if (x & (static_cast<T>(1) << i)) {
                        return std::numeric_limits<T>::digits - 1 - i;
                }
        }
        return std::numeric_limits<T>::digits;
}

template<class T>
constexpr auto popcount(T x) noexcept
{
        auto n = 0;
        for (; x; x &= x - static_cast<T>(1)) {
                ++n;
        }
        assert(n <= std::numeric_limits<T>::digits);
        return n;
}

template<class T>
constexpr auto bsfnz(T x)
{
        return ctznz(x);
}

template<class T>
constexpr auto bsrnz(T x)
{
        return std::numeric_limits<T>::digits - 1 - clznz(x);
}

template<class T>
constexpr auto ctz(T x) noexcept
{
        return ctznz(x);
}

template<class T>
constexpr auto clz(T x) noexcept
{
        return clznz(x);
}

template<class T>
constexpr auto bsf(T x)
{
        return bsfnz(x);
}

template<class T>
constexpr auto bsr(T x)
{
        return bsrnz(x);
}

}       // namespace loop
}       // namespace bit
}       // namespace xstd
