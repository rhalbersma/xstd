#pragma once
#include <type_traits>  // is_nothrow_move_constructible, is_nothrow-move_assignable
#include <utility>      // move

namespace xstd {

template<class T>
constexpr void swap(T& a, T& b) noexcept(
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_move_assignable<T>::value)
{
        T t(std::move(a));
        a = std::move(b);
        b = std::move(t);
}

}       // namespace xstd
