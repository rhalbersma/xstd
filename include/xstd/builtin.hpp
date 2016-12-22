#pragma once
#include <cassert>      // assert
#include <cstdint>      // uint64_t
#include <limits>       // digits

namespace xstd {
namespace builtin {
namespace detail {

// GCC / Clang have support for extended 128-bit integers.
// Uset get<0> and get<1> to extract the lower and upper 64-bit integers.

template<int N>
constexpr auto get(__uint128_t x) noexcept
{
        static_assert(0 <= N); static_assert(N < 2);
        return static_cast<uint64_t>(x >> (N * 64));
}

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

        constexpr auto operator()(__uint128_t x) const
        {
                if (auto const lower = get<0>(x)) {
                        return __builtin_ctzll(lower);
                } else {
                        return __builtin_ctzll(get<1>(x)) + 64;
                }
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

        constexpr auto operator()(__uint128_t x) const
        {
                if (auto const upper = get<1>(x)) {
                        return __builtin_clzll(upper);
                } else {
                        return __builtin_clzll(get<0>(x)) + 64;
                }
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

        constexpr auto operator()(__uint128_t x) const noexcept
        {
                return
                        __builtin_popcountll(get<0>(x)) +
                        __builtin_popcountll(get<1>(x))
                ;
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
constexpr auto popcount(T x) noexcept
{
        return detail::popcount{}(x);
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
constexpr auto bsf(T x)
{
        return ctz(x);
}

template<class T>
constexpr auto bsr(T x)
{
        return std::numeric_limits<T>::digits - 1 - clz(x);
}
}       // namespace builtin
}       // namespace xstd
