#pragma once
#include <xstd/word_array.hpp>   // word_array
#include <xstd/bit/mask.hpp>    // one
#include <xstd/bit/proxy.hpp>   // ConstIterator, ConstReference
#include <xstd/limits.hpp>      // digits
#include <cassert>              // assert
#include <cstdint>
#include <initializer_list>     // initializer_list
#include <iterator>             // reverse_iterator

namespace xstd {

template<class Block>
constexpr auto num_blocks(int N)
{
        return (N - 1 + digits<Block>) / digits<Block>;
}

using bitstorage = uint64_t;

template<int>
class int_set;

template<int N> bool operator==(int_set<N> const&, int_set<N> const&) noexcept;
template<int N> bool operator< (int_set<N> const&, int_set<N> const&) noexcept;
template<int N> bool intersects(int_set<N> const&, int_set<N> const&) noexcept;
template<int N> bool subset_of (int_set<N> const&, int_set<N> const&) noexcept;

template<int N>
class int_set
{
        using word_type = bitstorage;
        static constexpr auto Nb = num_blocks<word_type>(N);
        using Base = word_array<word_type, num_blocks<word_type>(N)>;

        Base m_words;

        friend bool operator==<>(int_set<N> const&, int_set<N> const&) noexcept;
        friend bool operator< <>(int_set<N> const&, int_set<N> const&) noexcept;
        friend bool intersects<>(int_set<N> const&, int_set<N> const&) noexcept;
        friend bool subset_of <>(int_set<N> const&, int_set<N> const&) noexcept;
public:
        // types:
        using value_type             = int;
        using pointer                = int*;
        using const_pointer          = int const*;
        using       reference        = bit::ConstReference<word_type, Nb, N>;
        using const_reference        = reference;
        using size_type              = int;
        using difference_type        = int;
        using       iterator         = bit::ConstIterator<word_type, Nb, N>;
        using const_iterator         = iterator;
        using       reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // constructors

        int_set() = default;

        template<class ForwardIterator>
        constexpr int_set(ForwardIterator first, ForwardIterator last) // Throws: Nothing.
        :
                m_words{}
        {
                for (auto it = first; it != last; ++it)
                        set(*it);
        }

        constexpr int_set(std::initializer_list<value_type> ilist) // Throws: Nothing.
        :
                int_set(ilist.begin(), ilist.end())
        {}

        // iterators
/*
        constexpr auto begin() noexcept
        {
                return iterator{m_words.begin()};
        }

        constexpr auto begin() const noexcept
        {
                return const_iterator{m_words.begin()};
        }

        constexpr auto end() noexcept
        {
                return iterator{m_words.end(), N};
        }

        constexpr auto end() const noexcept
        {
                return const_iterator{m_words.end(), N};
        }

        constexpr auto rbegin() noexcept
        {
                return reverse_iterator{end()};
        }

        constexpr auto rbegin() const noexcept
        {
                return const_reverse_iterator{end()};
        }

        constexpr auto rend() noexcept
        {
                return reverse_iterator{begin()};
        }

        constexpr auto rend() const noexcept
        {
                return const_reverse_iterator{begin()};
        }

        constexpr auto cbegin() const noexcept
        {
                return begin();
        }

        constexpr auto cend() const noexcept
        {
                return end();
        }

        constexpr auto crbegin() const noexcept
        {
                return rbegin();
        }

        constexpr auto crend() const noexcept
        {
                return rend();
        }
*/
        // capacity

        static constexpr auto size() noexcept
        {
                return N;
        }

        // modifiers

        auto& set() noexcept
        {
                m_words.fill(bit::mask::all<word_type>);
                sanitize();
                assert(all());
                return *this;
        }

        auto& reset() noexcept
        {
                m_words.fill(bit::mask::none<word_type>);
                assert(none());
                return *this;
        }

        auto swap(int_set& other) noexcept
        {
                m_words.swap(other);
        }

        // element access

        constexpr auto& set(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[which(n)] |= word(where(n));
                assert(test(n));
                return *this;
        }

        constexpr auto& reset(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[which(n)] &= ~word(where(n));
                assert(!test(n));
                return *this;
        }

        constexpr auto& flip(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[which(n)] ^= word(where(n));
                return *this;
        }

        constexpr auto test(int const n) const // Throws: Nothing.
                -> bool
        {
                assert(0 <= n); assert(n < N);
                return m_words[which(n)] & word(where(n));
        }

        // comparators

        // modifiers

        constexpr auto& flip() noexcept
        {
                m_words.flip();
                sanitize();
                return *this;
        }

        constexpr auto& operator&=(int_set const& other) noexcept
        {
                m_words &= other.m_words;
                return *this;
        }

        constexpr auto& operator|=(int_set const& other) noexcept
        {
                m_words |= other.m_words;
                return *this;
        }

        constexpr auto& operator^=(int_set const& other) noexcept
        {
                m_words ^= other.m_words;
                return *this;
        }

        constexpr auto& operator-=(int_set const& other) noexcept
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

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction f) const
        {
                return m_words.for_each(std::move(f));
        }

        template<class UnaryFunction>
        constexpr auto reverse_for_each(UnaryFunction f) const
        {
                return m_words.reverse_for_each(std::move(f));
        }

        // observers

        auto all() const noexcept
        {
                return m_words.template all<remaining_bits()>();
        }

        auto any() const noexcept
        {
                return m_words.any();
        }

        auto none() const noexcept
        {
                return m_words.none();
        }

        auto count() const noexcept
        {
                return m_words.count();
        }

private:
        constexpr auto which(int const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (N <= word_size()) {
                        return 0;
                } else {
                        return n / word_size();
                }
        }

        constexpr auto where(int const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (N <= word_size()) {
                        return n;
                } else {
                        return n % word_size();
                }
        }

        constexpr auto word(int const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < word_size());
                return bit::mask::one<word_type> << n;
        }

        static constexpr auto remaining_bits() noexcept
        {
                return N % word_size();
        }

        static constexpr auto word_size() noexcept
        {
                return Base::word_size();
        }

        constexpr auto sanitize() noexcept
        {
                if constexpr (remaining_bits() != 0) {
                        m_words.back() &= bit::mask::all<word_type> >> (word_size() - remaining_bits());
                }
        }
};
/*
template<int N>
constexpr auto
begin(int_set<N>& b) -> decltype(b.begin())
{
        return b.begin();
}

template<int N>
constexpr auto
begin(int_set<N> const& b) -> decltype(b.begin())
{
        return b.begin();
}

template<int N>
constexpr auto
end(int_set<N>& b) -> decltype(b.end())
{
        return b.end();
}

template<int N>
constexpr auto
end(int_set<N> const& b) -> decltype(b.end())
{
        return b.end();
}

template<int N>
constexpr auto
cbegin(int_set<N> const& b) noexcept(noexcept(std::begin(b))) -> decltype(xstd::begin(b))
{
        return xstd::begin(b);
}

template<int N>
constexpr auto
cend(int_set<N> const& b) noexcept(noexcept(std::end(b)))-> decltype(xstd::end(b))
{
        return xstd::end(b);
}

template<int N>
constexpr auto
rbegin(int_set<N>& b) -> decltype(b.rbegin())
{
        return b.rbegin();
}

template<int N>
constexpr auto
rbegin(int_set<N> const& b) -> decltype(b.rbegin())
{
        return b.rbegin();
}

template<int N>
constexpr auto
rend(int_set<N>& b) -> decltype(b.rend())
{
        return b.rend();
}

template<int N>
constexpr auto
rend(int_set<N> const& b) -> decltype(b.rend())
{
        return b.rend();
}

template<int N>
constexpr auto
crbegin(int_set<N> const& b) -> decltype(xstd::rbegin(b))
{
        return xstd::rbegin(b);
}

template<int N>
constexpr auto
crend(int_set<N> const& b) -> decltype(xstd::rend(b))
{
        return xstd::rend(b);
}
*/
template<int N>
bool operator==(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return lhs.m_words == rhs.m_words;
}

template<int N>
bool operator<(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return lhs.m_words < rhs.m_words;
}

template<int N>
auto operator!=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N>
auto operator>(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return rhs < lhs;
}

template<int N>
auto operator>=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<int N>
auto operator<=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template<int N>
auto swap(int_set<N>& lhs, int_set<N>& rhs) noexcept
{
        lhs.swap(rhs);
}

template<int N>
bool intersects(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return intersects(lhs.m_words, rhs.m_words);
}

template<int N>
auto disjoint(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

// <=
template<int N>
bool subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return subset_of(lhs.m_words, rhs.m_words);
}

// >=
template<int N>
auto superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return subset_of(rhs, lhs);
}

// <
template<int N>
auto proper_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return subset_of(lhs, rhs) && !subset_of(rhs, lhs);
}

// >
template<int N>
auto proper_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return superset_of(lhs, rhs) && !superset_of(rhs, lhs);
}

template<int N>
constexpr auto operator~(int_set<N> const& lhs) noexcept
{
        int_set<N> nrv{lhs}; nrv.flip(); return nrv;
}

template<int N>
constexpr auto operator&(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        int_set<N> nrv{lhs}; nrv &= rhs; return nrv;
}

template<int N>
constexpr auto operator|(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        int_set<N> nrv{lhs}; nrv |= rhs; return nrv;
}

template<int N>
constexpr auto operator^(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        int_set<N> nrv{lhs}; nrv ^= rhs; return nrv;
}

template<int N>
constexpr auto operator-(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        int_set<N> nrv{lhs}; nrv -= rhs; return nrv;
}

template<int N>
auto operator<<(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        int_set<N> nrv{lhs}; nrv <<= n; return nrv;
}

template<int N>
auto operator>>(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        int_set<N> nrv{lhs}; nrv >>= n; return nrv;
}

}       // namespace xstd
