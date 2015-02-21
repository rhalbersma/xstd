#pragma once
#include <xstd/bit/mask.hpp>    // one
#include <xstd/cstddef.hpp>     // _z
#include <xstd/limits.hpp>      // digits
#include <cassert>              // assert

namespace xstd {
namespace bit {
namespace loop {

template<class T>
constexpr auto ctz(T x) noexcept
{
        for (auto i = 0_z; i < digits<T>; ++i)
                if (x & (mask::one<T> << i))
                        return i;
        return digits<T>;
}

template<class T>
constexpr auto clz(T x) noexcept
{
        for (auto i = digits<T> - 1; i < digits<T>; --i)
                if (x & (mask::one<T> << i))
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
        auto n = 0_z;
        for (; x; x &= x - mask::one<T>)
                ++n;
        assert(n <= digits<T>);
        return n;
}

}       // namespace loop
}       // namespace bit
}       // namespace xstd