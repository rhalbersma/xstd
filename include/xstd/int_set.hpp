#pragma once
#include <xstd/bit/mask.hpp>                            // all, one, none
#include <xstd/bit/primitive.hpp>                       // ctznz, popcount
#include <xstd/limits.hpp>                              // digits
#include <hash_append/hash_append.h>                    // hash_append
#include <boost/algorithm/cxx11/all_of.hpp>             // all_of
#include <boost/algorithm/cxx11/any_of.hpp>             // any_of
#include <boost/algorithm/cxx11/none_of.hpp>            // none_of
#include <boost/iterator/iterator_facade.hpp>           // iterator_core_access, iterator_facade
#include <boost/range/algorithm.hpp>                    // copy_backward, equal, fill_n, lexicographical_compare
#include <boost/range/algorithm_ext.hpp>                // copy_n
#include <boost/range/algorithm/swap_ranges.hpp>        // swap_ranges
#include <boost/range/adaptors.hpp>                     // reversed, sliced
#include <boost/range/combine.hpp>                      // combine
#include <boost/range/numeric.hpp>                      // accumulate
#include <boost/tuple/tuple.hpp>                        // get
#include <cassert>                                      // assert
#include <functional>                                   // less
#include <initializer_list>                             // initializer_list
#include <iterator>                                     // bidirectional_iterator_tag, reverse_iterator
#include <type_traits>                                  // is_pod

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
        static_assert(0 <= N);

        static constexpr auto static_assert_type_traits() noexcept
        {
                static_assert(std::is_pod<int_set>{});
        }

        using WordT = unsigned long long;
        static_assert(std::is_unsigned<WordT>{});
        static_assert(std::is_integral<WordT>{});

        static constexpr auto word_size = digits<WordT>;
        static constexpr auto Nw = (N - 1 + word_size) / word_size;
        static constexpr auto Nb = Nw * word_size;
        static constexpr auto ExcessBits = Nb - N;

        // TODO: use std::array<WordT, Nw> when the STL gets non-const constexpr operator[]
        WordT m_words[Nw ? Nw : 1];
public:
        using key_type        = int;
        using key_compare     = std::less<>;
        using value_type      = int;
        using value_compare   = std::less<>;
        using size_type       = int;
        using difference_type = int;

        class const_reference;
        class const_iterator;

        class const_reference
        {
                constexpr auto assert_invariants() const noexcept
                {
                        assert(0 <= m_index); assert(m_index < N);
                }

                WordT const& m_word;
                size_type const m_index;
        public:
                const_reference() = delete;
                const_reference(const_reference const&) = default;
                const_reference& operator=(const_reference const&) = delete;
                const_reference& operator=(value_type const) = delete;

                constexpr const_reference(WordT const& w, size_type const i) noexcept
                :
                        m_word{w},
                        m_index{i}
                {
                        assert_invariants();
                }

                /* implicit */ constexpr operator value_type() const noexcept
                {
                        return m_index;
                }

                constexpr auto operator&() const noexcept
                        -> const_iterator
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
                constexpr auto assert_invariants() const noexcept
                {
                        assert(m_word != nullptr);
                        assert(0 <= m_index); assert(m_index <= N);
                }

                WordT const* m_word;
                size_type m_index;
        public:
                const_iterator() = default;

                explicit constexpr const_iterator(WordT const* w) // Throws: Nothing.
                :
                        m_word{w},
                        m_index{find_first()}
                {
                        assert_invariants();
                }

                constexpr const_iterator(WordT const* w, size_type const i) // Throws: Nothing.
                :
                        m_word{w},
                        m_index{i}
                {
                        assert_invariants();
                }

        private:
                constexpr auto find_first() noexcept
                {
                        if constexpr (Nw == 0) {
                                ++m_word;
                                return N;
                        } else if constexpr (Nw == 1) {
                                if (auto const word = *m_word) {
                                        return bit::ctznz(word);
                                } else {
                                        ++m_word;
                                        return N;
                                }
                        } else if constexpr (Nw >= 2) {
                                for (auto i = 0; i < Nw; ++i) {
                                        if (auto const word = *m_word) {
                                                assert(i * word_size + bit::bsfnz(word) < N);
                                                return i * word_size + bit::bsfnz(word);
                                        }
                                        ++m_word;
                                }
                                return N;
                        }
                }

                // gateway for boost::iterator_facade to access the implemenation
                // increment(), decrement(), dereference() and equal()
                friend class boost::iterator_core_access;

                // operator++() and operator++(int) provided by boost::iterator_facade
                constexpr auto increment() // Throws: Nothing.
                {
                        assert(0 <= m_index); assert(m_index < N);
                        if constexpr (Nw == 0) {
                                return;
                        } else if constexpr (Nw == 1) {
                                if (++m_index == N) {
                                        ++m_word;
                                        return;
                                }
                                if (auto const word = *m_word >> m_index) {
                                        m_index += bit::ctznz(word);
                                        return;
                                } else {
                                        ++m_word;
                                        m_index = N;
                                }
                                assert(0 < m_index && m_index <= N);
                        } else if constexpr (Nw >= 2) {
                                if (++m_index == N) {
                                        ++m_word;
                                        return;
                                }

                                auto const index = where(m_index);
                                if (index == 0) {
                                        ++m_word;
                                }
                                if (auto const word = *m_word >> index) {
                                        m_index += bit::ctznz(word);
                                        assert(m_index < N);
                                        return;
                                }
                                ++m_word;

                                for (auto i = which(m_index) + 1; i < Nw; ++i) {
                                        if (auto const word = *m_word) {
                                                m_index = i * word_size + bit::bsfnz(word);
                                                assert(m_index < N);
                                                return;
                                        }
                                        ++m_word;
                                }
                                m_index = N;
                                assert(0 < m_index && m_index <= N);
                        }
                }

                // operator--() and operator--(int) provided by boost::iterator_facade
                constexpr auto decrement() // Throws: Nothing.
                {
                        assert(0 < m_index); assert(m_index <= N);
                        if constexpr (Nw == 0) {
                                return;
                        } else if constexpr (Nw == 1) {
                                if (--m_index == 0) {
                                        return;
                                }
                                if (m_index == N - 1) {
                                        --m_word;
                                }
                                if (auto const word = *m_word << (word_size - 1 - m_index)) {
                                        m_index -= bit::clznz(word);
                                } else {
                                        m_index = 0;
                                }
                                assert(m_index < N);
                        } else if constexpr (Nw >= 2) {
                                if (--m_index == 0) {
                                        return;
                                }

                                auto const index = where(m_index);
                                if (index == word_size - 1 || m_index == N - 1) {
                                        --m_word;
                                }
                                if (auto const word = *m_word << (word_size - 1 - index)) {
                                        m_index -= bit::clznz(word);
                                        assert(m_index < N);
                                        return;
                                }
                                --m_word;

                                for (auto i = which(m_index) - 1; i >= 0; --i) {
                                        if (auto const word = *m_word) {
                                                m_index = i * word_size + bit::bsrnz(word);
                                                assert(m_index < N);
                                                return;
                                        }
                                        --m_word;
                                }
                                ++m_word;
                                m_index = 0;
                                assert(m_index < N);
                        }
                }

                // operator* provided by boost::iterator_facade
                constexpr auto dereference() const // Throws: Nothing.
                        -> const_reference
                {
                        assert(0 <= m_index); assert(m_index < N);
                        return { *m_word, m_index };
                }

                // operator== and operator!= provided by boost::iterator_facade
                constexpr auto equal(const_iterator const& other) const noexcept
                {
                        return m_word == other.m_word && m_index == other.m_index;
                }
        };

        using reference              = const_reference;
        using iterator               = const_iterator;
        using pointer                = iterator;
        using const_pointer          = const_iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using insert_return_type     = void;

        int_set() = default;

        template<class InputIterator>
        constexpr int_set(InputIterator first, InputIterator last) // Throws: Nothing.
        :
                m_words{}
        {
                insert(first, last);
        }

        constexpr int_set(std::initializer_list<value_type> ilist) // Throws: Nothing.
        :
                int_set(ilist.begin(), ilist.end())
        {}

        constexpr int_set& operator=(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                insert(ilist);
                return *this;
        }

        constexpr auto begin()         noexcept { using std::begin; return       iterator{begin(m_words)}; }
        constexpr auto begin()   const noexcept { using std::begin; return const_iterator{begin(m_words)}; }
        constexpr auto end()           noexcept { using std::end; return       iterator{end(m_words), N}; }
        constexpr auto end()     const noexcept { using std::end; return const_iterator{end(m_words), N}; }

        constexpr auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        constexpr auto rend()          noexcept { return       reverse_iterator{begin()}; }
        constexpr auto rend()    const noexcept { return const_reverse_iterator{begin()}; }

        constexpr auto cbegin()  const noexcept { return const_iterator{begin()}; }
        constexpr auto cend()    const noexcept { return const_iterator{end()};   }
        constexpr auto crbegin() const noexcept { return const_reverse_iterator{rbegin()}; }
        constexpr auto crend()   const noexcept { return const_reverse_iterator{rend()};   }

        bool empty() const noexcept
        {
                if constexpr (Nw == 0) {
                        return true;
                } else if constexpr (Nw == 1) {
                        return m_words[0] == bit::mask::none<WordT>;
                } else if constexpr (Nw >= 2) {
                        return boost::algorithm::none_of(m_words, [](auto const word){
                                return word != bit::mask::none<WordT>;
                        });
                }
        }

        bool full() const noexcept
        {
                if constexpr (ExcessBits == 0) {
                        if constexpr (Nw == 0) {
                                return true;
                        } else if constexpr (Nw == 1) {
                                return m_words[0] == bit::mask::all<WordT>;
                        } else if constexpr (Nw >= 2) {
                                return boost::algorithm::all_of(m_words, [](auto const word){
                                        return word == bit::mask::all<WordT>;
                                });
                        }
                } else {
                        static_assert(Nw != 0);
                        if constexpr (Nw == 1) {
                                return m_words[0] == bit::mask::all<WordT> >> ExcessBits;
                        } else if constexpr (Nw >= 2) {
                                using boost::adaptors::sliced;
                                return
                                        boost::algorithm::all_of(m_words | sliced(0, Nw - 1), [](auto const word){
                                                return word == bit::mask::all<WordT>;
                                        }) && (m_words[Nw - 1] == bit::mask::all<WordT> >> ExcessBits);
                                ;
                        }
                }
        }

        size_type size() const noexcept
        {
                if constexpr (Nw == 0) {
                        return 0;
                } else if constexpr (Nw == 1) {
                        return bit::popcount(m_words[0]);
                } else if (Nw >= 2) {
                        return boost::accumulate(m_words, 0, [](auto const sum, auto const word){
                                return sum + bit::popcount(word);
                        });
                }
        }

        static constexpr size_type max_size() noexcept { return N;  }
        static constexpr size_type capacity() noexcept { return Nb; }

        constexpr auto& insert(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[which(n)] |= word_mask(where(n));
                assert(test(n));
                return *this;
        }

        template<class InputIterator>
        constexpr auto insert(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                for (auto it = first; it != last; ++it) {
                        insert(*it);
                }
        }

        constexpr auto insert(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                insert(ilist.begin(), ilist.end());
        }

        auto& fill() noexcept
        {
                /*m_words.*/fill(bit::mask::all<WordT>);
                sanitize();
                assert(full());
                return *this;
        }

        constexpr auto& erase(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[which(n)] &= ~word_mask(where(n));
                assert(!test(n));
                return *this;
        }

        constexpr auto erase(const_iterator it) // Throws: Nothing.
        {
                erase(*it);
        }

        template<class InputIterator>
        constexpr auto erase(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                for (auto it = first; it != last; ++it) {
                        erase(*it);
                }
        }

        // TODO: libstdc++ 6.2 does not provide is_nothrow_swappable
        void swap(int_set& other) noexcept // (Nw == 0 || is_nothrow_swappable<value_type>{})
        {
                if constexpr (Nw == 1) {
                        using std::swap;
                        swap(m_words[0], other.m_words[0]);
                } else if constexpr (Nw >= 2) {
                        boost::range::swap_ranges(m_words, other.m_words);
                }
        }

        auto clear() noexcept
        {
                /*m_words.*/fill(bit::mask::none<WordT>);
                assert(empty());
        }

        constexpr auto& flip(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[which(n)] ^= word_mask(where(n));
                return *this;
        }

        constexpr auto test(value_type const n) const // Throws: Nothing.
                -> bool
        {
                assert(0 <= n); assert(n < N);
                return m_words[which(n)] & word_mask(where(n));
        }

        constexpr auto& flip() noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] = ~m_words[0];
                } else if constexpr (Nw >= 2) {
                        for (auto& word : m_words) {
                                word = ~word;
                        }
                }
                sanitize();
                return *this;
        }

        constexpr auto& operator&=(int_set const& other) noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] &= other.m_words[0];
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0; i < Nw; ++i) {
                                m_words[i] &= other.m_words[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(int_set const& other) noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] |= other.m_words[0];
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0; i < Nw; ++i) {
                                m_words[i] |= other.m_words[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(int_set const& other) noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] ^= other.m_words[0];
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0; i < Nw; ++i) {
                                m_words[i] ^= other.m_words[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(int_set const& other) noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] &= ~other.m_words[0];
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0; i < Nw; ++i) {
                                m_words[i] &= ~other.m_words[i];
                        }
                }
                return *this;
        }

        auto& operator<<=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (Nw == 1) {
                        m_words[0] <<= n;
                } else if constexpr (Nw >= 2) {
                        if (n == 0) return *this;

                        auto const n_block = n / word_size;
                        auto const L_shift = n % word_size;

                        if (L_shift == 0) {
                                using boost::adaptors::sliced; using boost::end;
                                boost::copy_backward(m_words | sliced(0, static_cast<std::size_t>(Nw - n_block)), end(m_words));
                        } else {
                                auto const R_shift = word_size - L_shift;

                                for (auto i = Nw - 1; i > n_block; --i) {
                                        m_words[i] =
                                                (m_words[i - n_block    ] << L_shift) |
                                                (m_words[i - n_block - 1] >> R_shift)
                                        ;
                                }
                                m_words[n_block] = m_words[0] << L_shift;
                        }
                        boost::fill_n(m_words, n_block, bit::mask::none<WordT>);
                }
                sanitize();
                return *this;
        }

        auto& operator>>=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (Nw == 1) {
                        m_words[0] >>= n;
                } else if constexpr (Nw >= 2) {
                        if (n == 0) return *this;

                        auto const n_block = n / word_size;
                        auto const R_shift = n % word_size;

                        if (R_shift == 0) {
                                using boost::adaptors::sliced; using boost::begin;
                                boost::copy_n(m_words | sliced(static_cast<std::size_t>(n_block), Nw), Nw - n_block, begin(m_words));
                        } else {
                                auto const L_shift = word_size - R_shift;

                                for (auto i = 0; i < Nw - 1 - n_block; ++i) {
                                        m_words[i] =
                                                (m_words[i + n_block    ] >> R_shift) |
                                                (m_words[i + n_block + 1] << L_shift)
                                        ;
                                }
                                m_words[Nw - 1 - n_block] = m_words[Nw - 1] >> R_shift;
                        }
                        using boost::adaptors::reversed;
                        boost::fill_n(m_words | reversed, n_block, bit::mask::none<WordT>);
                }
                return *this;
        }

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction f) const
        {
                if constexpr (Nw == 1) {
                        for (auto word = m_words[0]; word;) {
                                auto const first = bit::bsfnz(word);
                                f(first);
                                word ^= word_mask(first);
                        }
                } else if constexpr (Nw >= 2) {
                        for (auto i = 0, offset = 0; i < Nw; ++i, offset += word_size) {
                                for (auto word = m_words[i]; word;) {
                                        auto const first = bit::bsfnz(word);
                                        f(offset + first);
                                        word ^= word_mask(first);
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
                                word ^= word_mask(last);
                        }
                } else if constexpr (Nw >= 2) {
                        for (auto i = Nw - 1, offset = (size() - 1) * word_size; i >= 0; --i, offset -= word_size) {
                                for (auto word = m_words[i]; word;) {
                                        auto const last = bit::bsrnz(word);
                                        f(offset + last);
                                        word ^= word_mask(last);
                                }
                        }
                }
                return std::move(f);
        }

        template<class HashAlgorithm>
        friend auto hash_append(HashAlgorithm& h, int_set const& is)
        {
                using xstd::hash_append;
                hash_append(h, is.m_words);
        }

private:
        void fill(WordT const& u) noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] = u;
                } else if constexpr (Nw >= 2) {
                        boost::fill_n(m_words, Nw, u);
                }
        }

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

        constexpr auto word_mask(int const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < word_size);
                return bit::mask::one<WordT> << n;
        }

        constexpr auto sanitize() noexcept
        {
                if constexpr (ExcessBits != 0) {
                        m_words[Nw - 1] &= bit::mask::all<WordT> >> ExcessBits;
                }
        }

        friend bool operator== <>(int_set const&, int_set const&) noexcept;
        friend bool operator<  <>(int_set const&, int_set const&) noexcept;
        friend bool intersects <>(int_set const&, int_set const&) noexcept;
        friend bool subset_of  <>(int_set const&, int_set const&) noexcept;
};

template<int N>
bool operator==(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto Nw = int_set<N>::Nw;
        if constexpr (Nw == 0) {
                return true;
        } else if constexpr (Nw == 1) {
                return lhs.m_words[0] == rhs.m_words[0];
        } else if constexpr (Nw >= 2) {
                return boost::equal(lhs.m_words, rhs.m_words);
        }
}

template<int N>
auto operator!=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(lhs == rhs);
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

template<int N>
constexpr auto set_complement(int_set<N> const& lhs) noexcept
{
        return ~lhs;
}

template<int N>
constexpr auto set_intersection(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return lhs & rhs;
}

template<int N>
constexpr auto set_union(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return lhs | rhs;
}

template<int N>
constexpr auto set_symmetric_difference(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return lhs ^ rhs;
}

template<int N>
constexpr auto set_difference(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return lhs - rhs;
}

template<int N>
auto set_transform_plus(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        return lhs << n;
}

template<int N>
auto set_transform_minus(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        return lhs >> n;
}

template<int N>
bool operator<(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto Nw = int_set<N>::Nw;
        if constexpr (Nw == 0) {
                return false;
        } else if constexpr (Nw == 1) {
                return lhs.m_words[0] < rhs.m_words[0];
        } else if constexpr (Nw >= 2) {
                using boost::adaptors::reversed;
                return boost::lexicographical_compare(lhs.m_words | reversed, rhs.m_words | reversed);
        }
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
auto swap(int_set<N>& lhs, int_set<N>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
        lhs.swap(rhs);
}

template<int N>
bool intersects(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto Nw = int_set<N>::Nw;
        if constexpr (Nw == 0) {
                return false;
        } else if constexpr (Nw == 1) {
                return lhs.m_words[0] & rhs.m_words[0];
        } else if constexpr (Nw >= 2) {
                return boost::algorithm::any_of(boost::combine(lhs.m_words, rhs.m_words), [](auto const& block){
                        return boost::get<0>(block) & boost::get<1>(block);
                });
        }
}

template<int N>
auto disjoint(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<int N>
bool subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto Nw = int_set<N>::Nw;
        if constexpr (Nw == 0) {
                return true;
        } else if constexpr (Nw == 1) {
                return !(lhs.m_words[0] & ~rhs.m_words[0]);
        } else if constexpr (Nw >= 2) {
                return boost::algorithm::all_of(boost::combine(lhs.m_words, rhs.m_words), [](auto const& block){
                        return !(boost::get<0>(block) & ~boost::get<1>(block));
                });
        }
}

template<int N>
auto superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return subset_of(rhs, lhs);
}

template<int N>
auto proper_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return subset_of(lhs, rhs) && !subset_of(rhs, lhs);
}

template<int N>
auto proper_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return superset_of(lhs, rhs) && !superset_of(rhs, lhs);
}

template<int N>
constexpr auto begin(int_set<N>& is)
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N>
constexpr auto begin(int_set<N> const& is)
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N>
constexpr auto end(int_set<N>& is)
        -> decltype(is.end())
{
        return is.end();
}

template<int N>
constexpr auto end(int_set<N> const& is)
        -> decltype(is.end())
{
        return is.end();
}

template<int N>
constexpr auto cbegin(int_set<N> const& is) noexcept(noexcept(xstd::begin(is)))
        -> decltype(xstd::begin(is))
{
        return xstd::begin(is);
}

template<int N>
constexpr auto cend(int_set<N> const& is) noexcept(noexcept(xstd::end(is)))
        -> decltype(xstd::end(is))
{
        return xstd::end(is);
}

template<int N>
constexpr auto rbegin(int_set<N>& is)
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N>
constexpr auto rbegin(int_set<N> const& is)
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N>
constexpr auto rend(int_set<N>& is)
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N>
constexpr auto rend(int_set<N> const& is)
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N>
constexpr auto crbegin(int_set<N> const& is)
        -> decltype(xstd::rbegin(is))
{
        return xstd::rbegin(is);
}

template<int N>
constexpr auto crend(int_set<N> const& is)
        -> decltype(xstd::rend(is))
{
        return xstd::rend(is);
}

template<int N>
constexpr auto size(int_set<N> const& is)
        -> decltype(is.size())
{
        return is.size();
}

template<int N>
constexpr auto empty(int_set<N> const& is)
        -> decltype(is.empty())
{
        return is.empty();
}

template<int N>
constexpr auto data(int_set<N>& is)
        -> decltype(is.data())
{
        return is.data();
}

template<int N>
constexpr auto data(int_set<N> const& is)
        -> decltype(is.data())
{
        return is.data();
}

}       // namespace xstd
