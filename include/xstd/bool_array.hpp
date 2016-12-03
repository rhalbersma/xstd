#pragma once
#include <xstd/bit/mask.hpp>                    // one
#include <xstd/bit/primitive.hpp>               // ctznz, popcount
#include <xstd/limits.hpp>                      // digits
#include <xstd/type_traits.hpp>                 // is_trivial_special_members
#include <xstd/word_array.hpp>                  // word_array
#include <boost/iterator/iterator_facade.hpp>   // iterator_core_access, iterator_facade
#include <cassert>                              // assert
#include <initializer_list>                     // initializer_list
#include <iterator>                             // random_access_iterator_tag, reverse_iterator
#include <type_traits>                          // is_pod

namespace xstd {

template<int N>
struct bool_array
{
        static_assert(0 <= N);

        static constexpr auto static_assert_type_traits() noexcept
        {
                using T = bool_array;
                static_assert(is_trivial_special_members_v<T>);
                static_assert(std::is_pod<T>{});
        }

        using WordT = unsigned long long;
        static constexpr auto word_size = digits<WordT>;
        static constexpr auto Nw = (N - 1 + word_size) / word_size;
        static constexpr auto Nb = Nw * word_size;

        word_array<WordT, Nw> m_words;

        // types:
        using value_type = bool;
        using size_type = int ; // see 23.2
        using difference_type = int ; // see 23.2

        class const_reference;
        class const_iterator;

        // TODO: implement reference/iterator classes

        using reference = const_reference;
        using iterator  = const_iterator;
        using pointer = iterator;
        using const_pointer = const_iterator;

        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // no explicit construct/copy/destroy for aggregate type

        void fill(value_type const& u) noexcept
        {
                m_words.fill(u ? bit::mask::all<WordT> : bit::mask::none<WordT>);
        }

        void swap(bool_array& other) noexcept(noexcept(m_words.swap(other)))
        {
                m_words.swap(other);
        }

        // iterators:
        constexpr auto begin()         noexcept { return       iterator{m_words.begin(), 0}; }
        constexpr auto begin()   const noexcept { return const_iterator{m_words.begin(), 0}; }
        constexpr auto end()           noexcept { return       iterator{m_words.end(), N}; }
        constexpr auto end()     const noexcept { return const_iterator{m_words.end(), N}; }

        constexpr auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        constexpr auto rend()          noexcept { return       reverse_iterator{begin()}; }
        constexpr auto rend()    const noexcept { return const_reverse_iterator{begin()}; }

        constexpr auto cbegin()  const noexcept { return const_iterator{begin()}; }
        constexpr auto cend()    const noexcept { return const_iterator{end()};   }
        constexpr auto crbegin() const noexcept { return const_reverse_iterator{rbegin()}; }
        constexpr auto crend()   const noexcept { return const_reverse_iterator{rend()};   }

        // capacity:
        constexpr bool      empty()    const noexcept { return N == 0; }
        constexpr size_type size()     const noexcept { return N; }
        constexpr size_type max_size() const noexcept { return N; }
        constexpr size_type capacity() const noexcept { return Nb; }

        // TODO
        // element access:
        constexpr reference       operator[](size_type n)       ;
        constexpr const_reference operator[](size_type n) const ;
        constexpr reference       at(size_type n)       ;
        constexpr const_reference at(size_type n) const ;
        constexpr reference       front()       ;
        constexpr const_reference front() const ;
        constexpr reference       back()        ;
        constexpr const_reference back()  const ;
        constexpr pointer         data()       noexcept ;
        constexpr const_pointer   data() const noexcept ;

        constexpr auto& flip() noexcept
        {
                m_words.flip();
                sanitize();
                return *this;
        }

        constexpr auto& operator&=(bool_array const& other) noexcept
        {
                m_words &= other.m_words;
                return *this;
        }

        constexpr auto& operator|=(bool_array const& other) noexcept
        {
                m_words |= other.m_words;
                return *this;
        }

        constexpr auto& operator^=(bool_array const& other) noexcept
        {
                m_words ^= other.m_words;
                return *this;
        }

        constexpr auto& operator-=(bool_array const& other) noexcept
        {
                m_words -= other.m_words;
                return *this;
        }

        auto& operator<<=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words <<= n;
                sanitize();
                return *this;
        }

        auto& operator>>=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words >>= n;
                return *this;
        }

        auto count() const noexcept
        {
                return m_words.count();
        }

        auto all() const noexcept
        {
                return m_words.template all<Nb - N>();
        }

        auto any() const noexcept
        {
                return m_words.any();
        }

        auto none() const noexcept
        {
                return m_words.none();
        }

        template<class HashAlgorithm>
        friend auto hash_append(HashAlgorithm, bool_array const&)
        {
                // TODO
        }
};

template<int N>
auto operator==(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        return lhs.m_words == rhs.m_words;
}

template<int N>
auto operator!=(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N>
auto operator<(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        return lhs.m_words < rhs.m_words;
}

template<int N>
auto operator>(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        return rhs < lhs;
}

template<int N>
auto operator>=(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<int N>
auto operator<=(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template <int N>
void swap(bool_array<N>& lhs, bool_array<N>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
        lhs.swap(rhs);
}

template<int N>
auto intersects(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
        -> bool
{
        return intersects(lhs.m_words, rhs.m_words);
}

template<int N>
auto subset_of(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
        -> bool
{
        return subset_of(lhs.m_words, rhs.m_words);
}

template<int N>
constexpr auto operator~(bool_array<N> const& lhs) noexcept
{
        bool_array<N> nrv{lhs}; nrv.flip(); return nrv;
}

template<int N>
constexpr auto operator&(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        bool_array<N> nrv{lhs}; nrv &= rhs; return nrv;
}

template<int N>
constexpr auto operator|(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        bool_array<N> nrv{lhs}; nrv |= rhs; return nrv;
}

template<int N>
constexpr auto operator^(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        bool_array<N> nrv{lhs}; nrv ^= rhs; return nrv;
}

template<int N>
constexpr auto operator-(bool_array<N> const& lhs, bool_array<N> const& rhs) noexcept
{
        bool_array<N> nrv{lhs}; nrv -= rhs; return nrv;
}

template<int N>
auto operator<<(bool_array<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < Nw * lhs.word_size());
        bool_array<N> nrv{lhs}; nrv <<= n; return nrv;
}

template<int N>
auto operator>>(bool_array<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < Nw * lhs.word_size());
        bool_array<N> nrv{lhs}; nrv >>= n; return nrv;
}

// range access:

template<int N>
constexpr auto begin(bool_array<N>& ba)
        -> decltype(ba.begin())
{
        return ba.begin();
}

template<int N>
constexpr auto begin(bool_array<N> const& ba)
        -> decltype(ba)
{
        return ba.begin();
}

template<int N>
constexpr auto end(bool_array<N>& ba)
        -> decltype(ba.end())
{
        return ba.end();
}

template<int N>
constexpr auto end(bool_array<N> const& ba)
        -> decltype(ba.end())
{
        return ba.end();
}

template<int N>
constexpr auto cbegin(bool_array<N> const& ba) noexcept(noexcept(xstd::begin(ba)))
        -> decltype(xstd::begin(ba))
{
        return xstd::begin(ba);
}

template<int N>
constexpr auto cend(bool_array<N> const& ba) noexcept(noexcept(xstd::end(ba)))
        -> decltype(xstd::end(ba))
{
        return xstd::end(ba);
}

template<int N>
constexpr auto rbegin(bool_array<N>& ba)
        -> decltype(ba.rbegin())
{
        return ba.rbegin();
}

template<int N>
constexpr auto rbegin(bool_array<N> const& ba)
        -> decltype(ba.rbegin())
{
        return ba.rbegin();
}

template<int N>
constexpr auto rend(bool_array<N>& ba)
        -> decltype(ba.rend())
{
        return ba.rend();
}

template<int N>
constexpr auto rend(bool_array<N> const& ba)
        -> decltype(ba.rend())
{
        return ba.rend();
}

template<int N>
constexpr auto crbegin(bool_array<N> const& ba)
        -> decltype(xstd::rbegin(ba))
{
        return xstd::rbegin(ba);
}

template<int N>
constexpr auto crend(bool_array<N> const& ba)
        -> decltype(xstd::rend(ba))
{
        return xstd::rend(ba);
}

// container access:

template<int N>
constexpr auto size(bool_array<N> const& ba)
        -> decltype(ba.size())
{
        return ba.size();
}

template<int N>
constexpr auto empty(bool_array<N> const& ba)
        -> decltype(ba.empty())
{
        return ba.empty();
}

template<int N>
constexpr auto data(bool_array<N>& ba)
        -> decltype(ba.data())
{
        return ba.data();
}

template<int N>
constexpr auto data(bool_array<N> const& ba)
        -> decltype(ba.data())
{
        return ba.data();
}

}       // namespace xstd
