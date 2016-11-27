#pragma once
#include <cassert>      // assert
#include <limits>       // digits

namespace xstd {
namespace bit {
namespace builtin {
namespace detail {

// GCC / Clang have built-in functions for Count Trailing Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined.

struct ctznz
{
        constexpr auto operator()(unsigned x) const
        {
                return __builtin_ctz(x);
        }

        constexpr auto operator()(unsigned long x) const
        {
                return __builtin_ctzl(x);
        }

        constexpr auto operator()(unsigned long long x) const
        {
                return __builtin_ctzll(x);
        }
};

// GCC / Clang have built-in functions for Count Leading Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined

struct clznz
{
        constexpr auto operator()(unsigned x) const
        {
                return __builtin_clz(x);
        }

        constexpr auto operator()(unsigned long x) const
        {
                return __builtin_clzl(x);
        }

        constexpr auto operator()(unsigned long long x) const
        {
                return __builtin_clzll(x);
        }
};

// GCC / Clang have built-in functions for Population Count
// for unsigned, unsigned long and unsigned long long.

struct popcount
{
        constexpr auto operator()(unsigned x) const noexcept
        {
                return __builtin_popcount(x);
        }

        constexpr auto operator()(unsigned long x) const noexcept
        {
                return __builtin_popcountl(x);
        }

        constexpr auto operator()(unsigned long long x) const noexcept
        {
                return __builtin_popcountll(x);
        }
};

}       // namespace detail

template<class T>
constexpr auto ctznz(T x)
{
        assert(x != 0);
        return detail::ctznz{}(x);
}

template<class T>
constexpr auto clznz(T x)
{
        assert(x != 0);
        return detail::clznz{}(x);
}

template<class T>
constexpr auto ctz(T x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<T>::digits;
}

template<class T>
constexpr auto clz(T x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<T>::digits;
}

template<class T>
constexpr auto bsfnz(T x)
{
        assert(x != 0);
        return ctznz(x);
}

template<class T>
constexpr auto bsrnz(T x)
{
        assert(x != 0);
        return std::numeric_limits<T>::digits - 1 - clznz(x);
}

template<class T>
constexpr auto bsf(T x)
{
        return ctz(x);
}

template<class T>
constexpr auto bsr(T x)
{
        return std::numeric_limits<T>::digits - 1 - clz(x);
}

template<class T>
constexpr auto popcount(T x) noexcept
{
        return detail::popcount{}(x);
}

}       // namespace builtin
}       // namespace bit
}       // namespace xstd
