#pragma once
#include <xstd/algorithm.hpp>   // equal, swap_ranges, fill_n, lexicographical_compare
#include <xstd/iterator.hpp>    // reverse_iterator
#include <cstddef>              // size_t
#include <initializer_list>     // initializer_list
#include <stdexcept>            // out_of_range
#include <type_traits>          // integral_constant, is_nothrow_swappable
#include <utility>              // declval

namespace xstd {

template<class T, std::size_t N>
struct array
{
        // types:
        typedef T&                                      reference;
        typedef const T&                                const_reference;
        typedef T*                                      iterator;
        typedef const T*                                const_iterator;
        typedef size_t                                  size_type;
        typedef ptrdiff_t                               difference_type;
        typedef T                                       value_type;
        typedef value_type*                             pointer;
        typedef const value_type*                       const_pointer;
        typedef xstd::reverse_iterator<iterator>        reverse_iterator;
        typedef xstd::reverse_iterator<const_iterator>  const_reverse_iterator;

        value_type elems[N ? N : 1];    // exposition only

        // no explicit construct/copy/destroy for aggregate type

        constexpr void fill(const value_type& u) noexcept
        {
                xstd::fill_n(begin(), N, u);
        }

        constexpr void swap(const array<T, N>& y) noexcept(
                noexcept(swap(std::declval<T&>(), std::declval<T&>())))
        {
                xstd::swap_ranges(begin(), end(), y.begin());
        }

        // iterators:
        constexpr       iterator begin()       noexcept { return       iterator(elems); }
        constexpr const_iterator begin() const noexcept { return const_iterator(elems); }
        constexpr       iterator end()         noexcept { return       iterator(elems + N); }
        constexpr const_iterator end()   const noexcept { return const_iterator(elems + N); }

        constexpr       reverse_iterator rbegin()       noexcept { return       reverse_iterator(end()); }
        constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        constexpr       reverse_iterator rend()         noexcept { return       reverse_iterator(begin()); }
        constexpr const_reverse_iterator rend()   const noexcept { return const_reverse_iterator(begin()); }

        constexpr         const_iterator cbegin()  const noexcept { return begin(); }
        constexpr         const_iterator cend()    const noexcept { return end();   }
        constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        constexpr const_reverse_iterator crend()   const noexcept { return rend();   }

        // capacity:
        constexpr size_type size()     const noexcept { return N; }
        constexpr size_type max_size() const noexcept { return N; }
        constexpr size_type empty()    const noexcept { return N == 0; }

        // element access:
        constexpr       reference operator[](size_type n)       { return elems[n]; }
        constexpr const_reference operator[](size_type n) const { return elems[n]; }

        constexpr reference at(size_type n)
        {
                if (n >= N)
                        throw std::out_of_range("array::at");
                return elems[n];
        }

        constexpr const_reference at(size_type n) const
        {
                if (n >= N)
                        throw std::out_of_range("array::at");
                return elems[n];
        }

        constexpr       reference front()       { return elems[0]; }
        constexpr const_reference front() const { return elems[0]; }
        constexpr       reference back()        { return elems[N ? N - 1 : 0]; }
        constexpr const_reference back()  const { return elems[N ? N - 1 : 0]; }
};

template<class T, std::size_t N>
constexpr bool
operator==(const array<T, N>& x, const array<T, N>& y)
{
        return xstd::equal(x.begin(), x.end(), y.begin(), y.end());
}

template<class T, std::size_t N>
constexpr bool
operator!=(const array<T, N>& x, const array<T, N>& y)
{
        return !(x == y);
}

template<class T, std::size_t N>
constexpr bool
operator<(const array<T, N>& x, const array<T, N>& y)
{
        return xstd::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<class T, std::size_t N>
constexpr bool
operator>(const array<T, N>& x, const array<T, N>& y)
{
        return y < x;
}

template<class T, std::size_t N>
constexpr bool
operator<=(const array<T, N>& x, const array<T, N>& y)
{
        return !(y < x);
}

template<class T, std::size_t N>
constexpr bool
operator>=(const array<T, N>& x, const array<T, N>& y)
{
        return !(x < y);
}

template<class T, size_t N >
constexpr void
swap(array<T, N>& x, array<T, N>& y) noexcept(
        noexcept(x.swap(y)))
{
        x.swap(y);
}

template<class>
class tuple_size;

template<std::size_t, class>
class tuple_element;

template<class T, std::size_t N>
class tuple_size<array<T, N>>
:
        public std::integral_constant<std::size_t, N>
{};

template<std::size_t I, class T, std::size_t N>
class tuple_element<I, array<T, N>>
{
public:
        using type = T;
};

template<std::size_t I, class T, std::size_t N>
constexpr T&
get(array<T, N>& a) noexcept
{
        static_assert(I < N, "");
        return a.elems[I];
}

template<std::size_t I, class T, std::size_t N>
constexpr T&&
get(array<T, N>&& a) noexcept
{
        static_assert(I < N, "");
        return std::move(get<I>(a));
}

template<std::size_t I, class T, size_t N>
constexpr const T&
get(const array<T, N>& a) noexcept
{
        static_assert(I < N, "");
        return a.elems[I];
}

}       // namespace xstd
