#pragma once
#include <xstd/word_array/word_array_fwd.hpp>   // word_array
#include <xstd/word_array/word_array_N.hpp>
#include <xstd/word_array/word_array_1.hpp>
#include <cassert>

namespace xstd {

template<class T, int N>
auto operator!=(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<class T, int N>
auto operator>(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return rhs < lhs;
}

template<class T, int N>
auto operator>=(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<class T, int N>
auto operator<=(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template<class T, int N>
constexpr auto operator~(word_array<T, N> const& lhs) noexcept
{
        word_array<T, N> nrv{lhs}; nrv.flip(); return nrv;
}

template<class T, int N>
constexpr auto operator&(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        word_array<T, N> nrv{lhs}; nrv &= rhs; return nrv;
}

template<class T, int N>
constexpr auto operator|(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        word_array<T, N> nrv{lhs}; nrv |= rhs; return nrv;
}

template<class T, int N>
constexpr auto operator^(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        word_array<T, N> nrv{lhs}; nrv ^= rhs; return nrv;
}

template<class T, int N>
constexpr auto operator-(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        word_array<T, N> nrv{lhs}; nrv -= rhs; return nrv;
}

template<class T, int N>
auto operator<<(word_array<T, N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        word_array<T, N> nrv{lhs}; nrv <<= n; return nrv;
}

template<class T, int N>
auto operator>>(word_array<T, N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        word_array<T, N> nrv{lhs}; nrv >>= n; return nrv;
}

}       // namespace xstd
