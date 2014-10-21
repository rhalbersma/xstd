#pragma once
#include <xstd/detail/swap.hpp> // swap, swap_ranges

namespace xstd {

template<class T>
constexpr void
swap(T& a, T& b) noexcept(
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_move_assignable<T>::value);

template<class T, size_t N>
constexpr void
swap(T (&a)[N], T (&b)[N]) noexcept(
        noexcept(swap(*a, *b)))
{
        xstd::swap_ranges(a, a + N, b);
}

}       // namespace xstd
