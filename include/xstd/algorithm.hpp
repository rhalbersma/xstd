#pragma once
#include <xstd/detail/swap.hpp> // swap, swap_ranges
#include <xstd/iterator.hpp>    // distance
#include <functional>           // equal_to, less
#include <iterator>             // iterator_traits, input_iterator_tag, forward_iterator_tag, random_access_iterator_tag

namespace xstd {

// NOTE: BinaryPredicate cannot be a lambda in constexpr contexts
template<class InputIterator1, class InputIterator2, class BinaryPredicate>
constexpr bool
equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate pred)
{
        for (; first1 != last1; ++first1, ++first2)
                if (!pred(*first1, *first2))
                        return false;
        return true;
}

template <class InputIterator1, class InputIterator2>
constexpr bool
equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
        return xstd::equal(first1, last1, first2, std::equal_to<>());
}

// NOTE: BinaryPredicate cannot be a lambda in constexpr contexts
template<class InputIterator1, class InputIterator2, class BinaryPredicate>
constexpr bool
equal(InputIterator1 first1, InputIterator1 last1,
      InputIterator2 first2, InputIterator2 last2, BinaryPredicate pred,
      std::input_iterator_tag, std::input_iterator_tag)
{
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
                if (!pred(*first1, *first2))
                        return false;
        return first1 == last1 && first2 == last2;
}

// NOTE: BinaryPredicate cannot be a lambda in constexpr contexts
template<class RandomAccessIterator1, class RandomAccessIterator2, class BinaryPredicate>
constexpr bool
equal(RandomAccessIterator1 first1, RandomAccessIterator1 last1,
      RandomAccessIterator2 first2, RandomAccessIterator2 last2, BinaryPredicate pred,
      std::random_access_iterator_tag, std::random_access_iterator_tag)
{
        if (xstd::distance(first1, last1) != xstd::distance(first2, last2))
                return false;
        return xstd::equal(first1, last1, first2, pred);
}

// NOTE: BinaryPredicate cannot be a lambda in constexpr contexts
template<class InputIterator1, class InputIterator2, class BinaryPredicate>
constexpr bool
equal(InputIterator1 first1, InputIterator1 last1,
      InputIterator2 first2, InputIterator2 last2, BinaryPredicate pred)
{
        typedef typename std::iterator_traits<InputIterator1>::iterator_category cat1;
        typedef typename std::iterator_traits<InputIterator2>::iterator_category cat2;
        return xstd::equal(first1, last1, first2, last2, pred, cat1(), cat2());
}

template <class InputIterator1, class InputIterator2>
constexpr bool
equal(InputIterator1 first1, InputIterator1 last1,
      InputIterator2 first2, InputIterator2 last2)
{
        typedef typename std::iterator_traits<InputIterator1>::iterator_category cat1;
        typedef typename std::iterator_traits<InputIterator2>::iterator_category cat2;
        return xstd::equal(first1, last1, first2, last2, std::equal_to<>(), cat1(), cat2());
}

template<class ForwardIterator1, class ForwardIterator2>
constexpr ForwardIterator2
swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2);

template<class ForwardIterator1, class ForwardIterator2>
constexpr void
iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
        swap(*a, *b);
}

template<class OutputIterator, class Size, class T>
constexpr OutputIterator
fill_n(OutputIterator first, Size n, const T& value)
{
        for (; n > 0; ++first, --n)
                *first = value;
        return first;
}

// NOTE: missing <char*, Size, char> overload that could delegate to std::memset

template<class ForwardIterator, class T>
constexpr void
fill(ForwardIterator first, ForwardIterator last, const T& value, std::forward_iterator_tag)
{
        for (; first != last; ++first)
                *first = value;
}

template <class RandomAccessIterator, class T>
constexpr void
fill(RandomAccessIterator first, RandomAccessIterator last, const T& value, std::random_access_iterator_tag)
{
        xstd::fill_n(first, last - first, value);
}

template<class ForwardIterator, class T>
constexpr void
fill(ForwardIterator first, ForwardIterator last, const T& value)
{
        typedef typename std::iterator_traits<ForwardIterator>::iterator_category cat;
        xstd::fill(first, last, value, cat());
}

template<class ForwardIterator, class Compare>
constexpr ForwardIterator
min_element(ForwardIterator first, ForwardIterator last, Compare comp)
{
        if (first != last) {
                ForwardIterator i = first;
                while (++i != last)
                        if (comp(*i, *first))
                                first = i;
        }
        return first;
}

template<class ForwardIterator>
constexpr ForwardIterator
min_element(ForwardIterator first, ForwardIterator last)
{
        return xstd::min_element(first, last, std::less<>());
}

template<class ForwardIterator, class Compare>
constexpr ForwardIterator
max_element(ForwardIterator first, ForwardIterator last, Compare comp)
{
        if (first != last) {
                ForwardIterator i = first;
                while (++i != last)
                        if (comp(*first, *i))
                                first = i;
        }
        return first;
}

template<class ForwardIterator>
constexpr ForwardIterator
max_element(ForwardIterator first, ForwardIterator last)
{
        return xstd::max_element(first, last, std::less<>());
}

template<class InputIterator1, class InputIterator2, class Compare>
constexpr bool
lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2, Compare comp)
{
        for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
                if (comp(*first1, *first2))
                        return true;
                if (comp(*first2, *first1))
                        return false;
        }
        return false;
}

template<class InputIterator1, class InputIterator2>
constexpr bool
lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2)
{
        return xstd::lexicographical_compare(first1, last1, first2, last2, std::less<>());
}

}       // namespace xstd
