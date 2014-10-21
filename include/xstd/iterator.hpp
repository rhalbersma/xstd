#pragma once
#include <iterator>     // iterator, iterator_traits, input_iterator_tag, random_access_iterator_tag
#include <memory>       // addressof

namespace xstd {

template<class InputIterator>
constexpr typename std::iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last, std::input_iterator_tag)
{
        typename std::iterator_traits<InputIterator>::difference_type r(0);
        for (; first != last; ++first)
                ++r;
        return r;
}

template<class RandomAccessIterator>
constexpr typename std::iterator_traits<RandomAccessIterator>::difference_type
distance(RandomAccessIterator first, RandomAccessIterator last, std::random_access_iterator_tag)
{
        return last - first;
}

template<class InputIterator>
constexpr typename std::iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
        typedef typename std::iterator_traits<InputIterator>::iterator_category cat;
        return xstd::distance(first, last, cat());
}

template<class Iterator>
class reverse_iterator
:
        public std::iterator<
                typename std::iterator_traits<Iterator>::iterator_category,
                typename std::iterator_traits<Iterator>::value_type,
                typename std::iterator_traits<Iterator>::difference_type,
                typename std::iterator_traits<Iterator>::pointer,
                typename std::iterator_traits<Iterator>::reference
        >
{
public:
        typedef Iterator                                                        iterator_type;
        typedef typename std::iterator_traits<Iterator>::difference_type        difference_type;
        typedef typename std::iterator_traits<Iterator>::reference              reference;
        typedef typename std::iterator_traits<Iterator>::pointer                pointer;

        constexpr reverse_iterator(): current() {}
        constexpr explicit reverse_iterator(Iterator x): current(x) {}

        template <class U>
        constexpr reverse_iterator(const reverse_iterator<U>& u)
        :
                current(u.current)
        {}

        template <class U>
        constexpr reverse_iterator& operator=(const reverse_iterator<U>& u)
        {
                current = u.base(); return *this;
        }

        constexpr Iterator base() const { return current; } // explicit
        constexpr reference operator*()  const { Iterator tmp = current; return *--tmp; }
        constexpr pointer   operator->() const { return std::addressof(operator*());    }

        constexpr reverse_iterator& operator++()    {                               --current; return *this; }
        constexpr reverse_iterator  operator++(int) { reverse_iterator tmp = *this; --current; return tmp;   }
        constexpr reverse_iterator& operator--()    {                               ++current; return *this; }
        constexpr reverse_iterator  operator--(int) { reverse_iterator tmp = *this; ++current; return tmp;   }

        constexpr reverse_iterator  operator+ (difference_type n) const { return reverse_iterator(current - n); }
        constexpr reverse_iterator& operator+=(difference_type n)       { current -= n; return *this; }
        constexpr reverse_iterator  operator- (difference_type n) const { return reverse_iterator(current + n); }
        constexpr reverse_iterator& operator-=(difference_type n)       { current += n; return *this; }
        constexpr reference         operator[](difference_type n) const { return current[-n - 1]; }

protected:
        Iterator current;
};

template<class Iterator1, class Iterator2>
constexpr bool
operator==(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
{
        return x.base() == y.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator<(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
{
        return x.base() > y.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator!=(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
{
        return x.base() != y.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator>(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
{
        return x.base() < y.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator>=(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
{
        return x.base() <= y.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator<=(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
{
        return x.base() >= y.base();
}

template<class Iterator1, class Iterator2>
constexpr auto
operator-(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y)
-> decltype(y.base() - x.base())
{
        return y.base() - x.base();
}

template<class Iterator>
constexpr reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& x)
{
        reverse_iterator<Iterator>(x.base() - n);
}

template<class Iterator>
constexpr reverse_iterator<Iterator>
make_reverse_iterator(Iterator i)
{
        return reverse_iterator<Iterator>(i);
}

}       // namespace xstd
