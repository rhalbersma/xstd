#pragma once
#include <xstd/word_array.hpp>                  // word_array
#include <xstd/bit/mask.hpp>                    // one
#include <xstd/bit/primitive.hpp>               // ctznz, popcount
#include <boost/iterator/iterator_facade.hpp>   // iterator_core_access, iterator_facade
#include <cassert>                              // assert
#include <cstdint>                              // uint64_t
#include <initializer_list>                     // initializer_list
#include <iterator>                             // reverse_iterator
#include <limits>                               // digits

namespace xstd {

template<int>
class int_set;

template<int N> bool operator==(int_set<N> const&, int_set<N> const&) noexcept;
template<int N> bool operator< (int_set<N> const&, int_set<N> const&) noexcept;
template<int N> bool intersects(int_set<N> const&, int_set<N> const&) noexcept;
template<int N> bool subset_of (int_set<N> const&, int_set<N> const&) noexcept;

template<int N>
class int_set
{
        using WordT = uint64_t;
        static constexpr auto word_size = std::numeric_limits<WordT>::digits;
        static constexpr auto Nw = (N - 1 + word_size) / word_size;
        static constexpr auto Nb = Nw * word_size;

        word_array<WordT, Nw> m_words;

        constexpr auto find_first() const noexcept
        {
                if constexpr (Nw == 0) {
                        return 0;
                } else if constexpr (Nw == 1) {
                        return bit::ctz(m_words[0]);
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0; i < Nw; ++i) {
                                if (auto const word = m_words[i]) {
                                        return i * word_size + bit::ctznz(word);
                                }
                        }
                        return Nb;
                }
        }

public:
        // types:
        using value_type             = int;
        using pointer                = int*;
        using const_pointer          = int const*;
        using size_type              = int;
        using difference_type        = int;

        class const_reference;
        class const_iterator;

        class const_reference
        {
                auto assert_invariant() const
                {
                        assert(0 <= m_index); assert(m_index < N);
                }

                WordT const& m_word;
                value_type m_index;
        public:
                const_reference() = delete;
                const_reference(const_reference const&) = default;
                const_reference& operator=(const_reference const&) = delete;
                const_reference& operator=(value_type) = delete;

                constexpr const_reference(WordT const& w, value_type n) noexcept
                :
                        m_word{w},
                        m_index{n}
                {
                        assert_invariant();
                }

                /* implicit */ constexpr operator auto() const noexcept
                {
                        return m_index;
                }

                constexpr const_iterator operator&() const noexcept
                {
                        return { &m_word, m_index };
                }
        };

        class const_iterator
        :
                public boost::iterator_facade
                <
                        const_iterator,
                        value_type const,
                        std::bidirectional_iterator_tag,
                        const_reference,
                        difference_type
                >
        {
                auto assert_invariant() const
                {
                        assert(0 <= m_index); assert(m_index <= N);
                }

                WordT const* m_word;
                value_type m_index;
        public:
                const_iterator() = default;

                constexpr const_iterator(WordT const* w, value_type n)
                :
                        m_word{w},
                        m_index{n}
                {
                        assert_invariant();
                }

        private:
                // gateway for boost::iterator_facade to access private implementation
                friend class boost::iterator_core_access;

                // operator++() and operator++(int) provided by boost::iterator_facade
                constexpr auto increment()
                {
                        assert(m_word != nullptr);
                        assert(m_index < N);

                        if (++m_index == N) {
                                ++m_word;
                                return;
                        }
                        if constexpr (Nw == 1) {
                                assert(which(m_index) == 0); assert(!where(m_index));
                                if (auto const word = *m_word >> m_index) {
                                        m_index += bit::ctznz(word);
                                        return;
                                }
                                ++m_word;
                                m_index = N;
                        } else if constexpr (Nw >= 2) {
                                if (auto const index = where(m_index)) {
                                        if (auto const word = *m_word >> index) {
                                                m_index += bit::ctznz(word);
                                                return;
                                        }
                                        m_index += word_size - index;
                                        assert(!where(m_index));
                                }
                                for (++m_word; m_index < N; ++m_word, m_index += word_size) {
                                        if (*m_word) {
                                                m_index += bit::ctznz(*m_word);
                                                return;
                                        }
                                }
                                if constexpr (where(N)) {
                                        m_index = N;
                                }
                        }
                }

                // operator--() and operator--(int) provided by boost::iterator_facade
                constexpr auto decrement()
                {
                        assert(m_word != nullptr);
                        assert(0 < m_index);

                        if (m_index-- == N) {
                                --m_word;
                        } else if (m_index == 0) {
                                return;
                        }
                        if constexpr (Nw == 1) {
                                assert(which(m_index) == 0); assert(!where(m_index));
                                if (auto const word = *m_word << (word_size - 1 - m_index)) {
                                        m_index -= bit::clznz(word);
                                        return;
                                }
                                m_index = 0;
                        } else if constexpr (Nw >= 2) {
                                if (auto const index = where(m_index)) {
                                        if (auto const word = *m_word << (word_size - 1 - index)) {
                                                m_index -= bit::clznz(word);
                                                return;
                                        }
                                        m_index;
                                }
                                --m_word;
                                for (; m_index > 0; --m_word, m_index -= word_size) {
                                        if (auto const word = *m_word) {
                                                m_index -= bit::clznz(*m_word);
                                                return;
                                        }
                                }
                        }

                        assert_invariant();
                        assert(m_index < N);
                }

                // operator* provided by boost::iterator_facade
                constexpr const_reference dereference() const
                {
                        assert(m_index != N);
                        return { *m_word, m_index };
                }

                // operator== and operator!= provided by boost::iterator_facade
                constexpr auto equal(const_iterator const& other) const noexcept
                {
                        return m_word == other.m_word && m_index == other.m_index;
                }
        };

        using reference = const_reference;
        using iterator  = const_iterator;

        using reverse_iterator       = std::reverse_iterator<iterator>;
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

        constexpr auto begin()         noexcept { return       iterator{m_words.begin(), find_first()}; }
        constexpr auto begin()   const noexcept { return const_iterator{m_words.begin(), find_first()}; }
        constexpr auto end()           noexcept { return       iterator{m_words.end(), Nb}; }
        constexpr auto end()     const noexcept { return const_iterator{m_words.end(), Nb}; }
        constexpr auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        constexpr auto rend()          noexcept { return       reverse_iterator{begin()}; }
        constexpr auto rend()    const noexcept { return const_reverse_iterator{begin()}; }
        constexpr auto cbegin()  const noexcept { return begin(); }
        constexpr auto cend()    const noexcept { return end();   }
        constexpr auto crbegin() const noexcept { return rbegin(); }
        constexpr auto crend()   const noexcept { return rend();   }

        // capacity

        static constexpr auto size() noexcept
        {
                return N;
        }

        // modifiers

        auto& set() noexcept
        {
                m_words.fill(bit::mask::all<WordT>);
                sanitize();
                assert(all());
                return *this;
        }

        auto& reset() noexcept
        {
                m_words.fill(bit::mask::none<WordT>);
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
                if constexpr (Nw == 1) {
                        for (auto word = m_words[0]; word;) {
                                auto const first = bit::bsfnz(word);
                                f(first);
                                word ^= bit::mask::one<value_type> << first;
                        }
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0, offset = 0; i < Nw; ++i, offset += word_size) {
                                for (auto word = m_words[i]; word;) {
                                        auto const first = bit::bsfnz(word);
                                        f(offset + first);
                                        word ^= bit::mask::one<value_type> << first;
                                }
                        }
                }
                return std::move(f);
        }

        template<class UnaryFunction>
        constexpr auto reverse_for_each(UnaryFunction f) const
        {
                if constexpr (Nw == 1) {
                        for (auto word = m_words[0]; word;) {
                                auto const last = bit::bsrnz(word);
                                f(last);
                                word ^= bit::mask::one<value_type> << last;
                        }
                } else if constexpr (Nw >= 2) {
                        for (auto i = Nw - 1, offset = (size() - 1) * word_size; i >= 0; --i, offset -= word_size) {
                                for (auto word = m_words[i]; word;) {
                                        auto const last = bit::bsrnz(word);
                                        f(offset + last);
                                        word ^= bit::mask::one<value_type> << last;
                                }
                        }
                }
                return std::move(f);
        }

        // observers

        auto all() const noexcept
        {
                if constexpr (remaining_bits() == 0) {
                        return m_words.all();
                } else {
                        return m_words.all();//.template all<remaining_bits()>();
                }
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
        static constexpr auto which(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (N <= word_size) {
                        return 0;
                } else {
                        return n / word_size;
                }
        }

        static constexpr auto where(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (N <= word_size) {
                        return n;
                } else {
                        return n % word_size;
                }
        }

        constexpr auto word(int const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < word_size);
                return bit::mask::one<WordT> << n;
        }

        static constexpr auto remaining_bits() noexcept
        {
                return word_size - N % word_size;
        }

        constexpr auto sanitize() noexcept
        {
                if constexpr (remaining_bits() != 0) {
                        m_words.back() &= bit::mask::all<WordT> >> (word_size - remaining_bits());
                }
        }

        friend bool operator==<>(int_set<N> const&, int_set<N> const&) noexcept;
        friend bool operator< <>(int_set<N> const&, int_set<N> const&) noexcept;
        friend bool intersects<>(int_set<N> const&, int_set<N> const&) noexcept;
        friend bool subset_of <>(int_set<N> const&, int_set<N> const&) noexcept;
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

template<class WordT, int Nw>
auto operator<(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        if constexpr (Nw == 0) {
                return false;
        } else if constexpr (Nw == 1) {
                return lhs.m_words[0] < rhs.m_words[0];
        } else if constexpr (Nw == 2) {
                using boost::adaptors::reverse;
                return boost::lexicographical_compare(reverse(lhs.m_words), reverse(rhs.m_words));
        }
}

template<class WordT, int Nw>
auto intersects(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        return boost::algorithm::any_of(boost::combine(lhs.m_words, rhs.m_words), [](auto const& block){
                return boost::get<0>(block) & boost::get<1>(block);
        });
}

template<class WordT, int Nw>
auto subset_of(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        return boost::algorithm::all_of(boost::combine(lhs.m_words, rhs.m_words), [](auto const& block){
                return !(boost::get<0>(block) & ~boost::get<1>(block));
        });
}


template<class WordT, int Nw>
auto operator>(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        return rhs < lhs;
}

template<class WordT, int Nw>
auto operator>=(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<class WordT, int Nw>
auto operator<=(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        return !(rhs < lhs);
}


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
