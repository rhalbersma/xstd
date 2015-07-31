#pragma once
#include <type_traits>  // is_nothrow_move_constructible, is_nothrow_move_assignable
#include <utility>      // move

namespace xstd {

template<class T>
constexpr void
swap(T& a, T& b) noexcept(
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_move_assignable<T>::value)
{
        T t(std::move(a));
        a = std::move(b);
        b = std::move(t);
}

template<class ForwardIterator1, class ForwardIterator2>
constexpr ForwardIterator2
swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)
{
        using xstd::swap;
        for (; first1 != last1; ++first1, ++first2)
                swap(*first1, *first2);
        return first2;
}

}       // namespace xstd
