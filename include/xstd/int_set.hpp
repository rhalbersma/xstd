#pragma once
#include <algorithm>            // all_of, copy_backward, copy_n, equal, fill_n, lexicographical_compare, swap_ranges
#include <cassert>              // assert
#include <climits>              // CHAR_BIT
#include <cstdint>              // uint64_t
#include <functional>           // less
#include <initializer_list>     // initializer_list
#include <iterator>             // begin, bidirectional_iterator_tag, cbegin, cend, crbegin, crend, end, next, prev, rbegin, reverse_iterator
#include <limits>               // digits
#include <numeric>              // accumulate
#include <tuple>                // tie
#include <type_traits>          // is_integral, is_pod, is_unsigned

namespace xstd {
namespace builtin {
namespace detail {

// gcc / Clang have support for extended 128-bit integers.
// Uset get<0> and get<1> to extract the lower and upper 64-bit integers.

template<int N>
constexpr auto get(__uint128_t x) noexcept
{
        static_assert(0 <= N); static_assert(N < 2);
        return static_cast<uint64_t>(x >> (N * 64));
}

// gcc / Clang have built-in functions for Count Trailing Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined.

struct ctznz
{
        constexpr auto operator()(unsigned x) const
        {
                return __builtin_ctz(x);
        }

        constexpr auto operator()(unsigned long x) const
        {
                return __builtin_ctzl(x);
        }

        constexpr auto operator()(unsigned long long x) const
        {
                return __builtin_ctzll(x);
        }

        constexpr auto operator()(__uint128_t x) const
        {
                if (auto const lower = get<0>(x)) {
                        return __builtin_ctzll(lower);
                } else {
                        return __builtin_ctzll(get<1>(x)) + 64;
                }
        }
};

// gcc / Clang have built-in functions for Count Leading Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined

struct clznz
{
        constexpr auto operator()(unsigned x) const
        {
                return __builtin_clz(x);
        }

        constexpr auto operator()(unsigned long x) const
        {
                return __builtin_clzl(x);
        }

        constexpr auto operator()(unsigned long long x) const
        {
                return __builtin_clzll(x);
        }

        constexpr auto operator()(__uint128_t x) const
        {
                if (auto const upper = get<1>(x)) {
                        return __builtin_clzll(upper);
                } else {
                        return __builtin_clzll(get<0>(x)) + 64;
                }
        }
};

// gcc / Clang have built-in functions for Population Count
// for unsigned, unsigned long and unsigned long long.

struct popcount
{
        constexpr auto operator()(unsigned x) const noexcept
        {
                return __builtin_popcount(x);
        }

        constexpr auto operator()(unsigned long x) const noexcept
        {
                return __builtin_popcountl(x);
        }

        constexpr auto operator()(unsigned long long x) const noexcept
        {
                return __builtin_popcountll(x);
        }

        constexpr auto operator()(__uint128_t x) const noexcept
        {
                return
                        __builtin_popcountll(get<0>(x)) +
                        __builtin_popcountll(get<1>(x))
                ;
        }
};

}       // namespace detail

template<class T>
constexpr auto ctznz(T x)
{
        assert(x != 0);
        return detail::ctznz{}(x);
}

template<class T>
constexpr auto clznz(T x)
{
        assert(x != 0);
        return detail::clznz{}(x);
}

template<class T>
constexpr auto popcount(T x) noexcept
{
        return detail::popcount{}(x);
}

template<class T>
constexpr auto bsfnz(T x)
{
        assert(x != 0);
        return ctznz(x);
}

template<class T>
constexpr auto bsrnz(T x)
{
        assert(x != 0);
        return std::numeric_limits<T>::digits - 1 - clznz(x);
}

template<class T>
constexpr auto ctz(T x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<T>::digits;
}

template<class T>
constexpr auto clz(T x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<T>::digits;
}

template<class T>
constexpr auto bsf(T x)
{
        return ctz(x);
}

template<class T>
constexpr auto bsr(T x)
{
        return std::numeric_limits<T>::digits - 1 - clz(x);
}
}       // namespace builtin

template<int>
class int_set;

template<int N> auto operator==  (int_set<N> const&, int_set<N> const&) noexcept;
template<int N> auto operator<   (int_set<N> const&, int_set<N> const&) noexcept;
template<int N> auto intersects  (int_set<N> const&, int_set<N> const&) noexcept;
template<int N> auto is_subset_of(int_set<N> const&, int_set<N> const&) noexcept;

template<int N>
class int_set
{
        static_assert(0 <= N);

        static constexpr auto static_assert_type_traits() noexcept
        {
                static_assert(std::is_pod<int_set>{});
        }

        using word_type = unsigned long long;
        static_assert(std::is_unsigned<word_type>{});
        static_assert(std::is_integral<word_type>{});

        static constexpr auto word_size = std::numeric_limits<word_type>::digits;
        static constexpr auto num_words = (N - 1 + word_size) / word_size;
        static constexpr auto num_bits = num_words * word_size;
        static constexpr auto excess_bits = num_bits - N;

        word_type m_words[num_words ? num_words : 1];
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

                word_type const& m_word;
                size_type const m_index;
        public:
                const_reference() = delete;
                const_reference(const_reference const&) = default;
                const_reference& operator=(const_reference const&) = delete;
                const_reference& operator=(value_type const) = delete;

                constexpr const_reference(word_type const& w, size_type const i) noexcept
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
        {
                constexpr auto assert_invariants() const noexcept
                {
                        assert(m_word != nullptr);
                        assert(0 <= m_index); assert(m_index <= N);
                }

                word_type const* m_word;
                size_type m_index;
        public:
                using difference_type   = int_set::difference_type;
                using value_type        = int_set::value_type;
                using pointer           = const_iterator;
                using reference         = const_reference;
                using iterator_category = std::bidirectional_iterator_tag;

                const_iterator() = default;

                explicit constexpr const_iterator(word_type const* w) // Throws: Nothing.
                :
                        m_word{w},
                        m_index{find_first()}
                {
                        assert_invariants();
                }

                constexpr const_iterator(word_type const* w, size_type const i) // Throws: Nothing.
                :
                        m_word{w},
                        m_index{i}
                {
                        assert_invariants();
                }

                constexpr auto operator*() const // Throws: Nothing.
                        -> const_reference
                {
                        assert(0 <= m_index); assert(m_index < N);
                        return { *m_word, m_index };
                }

                constexpr auto& operator++() // Throws: Nothing.
                {
                        increment();
                        return *this;
                }

                constexpr auto operator++(int) // Throws: Nothing.
                {
                        auto nrv = *this; ++*this; return nrv;
                }

                constexpr auto& operator--() // Throws:Nothing.
                {
                        decrement();
                        return *this;
                }

                constexpr auto operator--(int) // Throws: Nothing.
                {
                        auto nrv = *this; --*this; return nrv;
                }

                friend constexpr auto operator==(const_iterator lhs, const_iterator rhs) noexcept
                {
                        return lhs.tied() == rhs.tied();
                }

                friend constexpr auto operator!=(const_iterator lhs, const_iterator rhs) noexcept
                {
                        return !(lhs == rhs);
                }
        private:
                constexpr auto find_first() noexcept
                {
                        if constexpr (num_words == 0) {
                                ++m_word;
                                return N;
                        } else if constexpr (num_words == 1) {
                                if (auto const word = *m_word) {
                                        return builtin::ctznz(word);
                                } else {
                                        ++m_word;
                                        return N;
                                }
                        } else if constexpr (num_words >= 2) {
                                for (auto i = 0; i < num_words; ++i) {
                                        if (auto const word = *m_word) {
                                                assert(i * word_size + builtin::bsfnz(word) < N);
                                                return i * word_size + builtin::bsfnz(word);
                                        }
                                        ++m_word;
                                }
                                return N;
                        }
                }

                constexpr auto increment() // Throws: Nothing.
                {
                        assert(0 <= m_index); assert(m_index < N);
                        if constexpr (num_words == 0) {
                                return;
                        } else if constexpr (num_words == 1) {
                                if (++m_index == N) {
                                        ++m_word;
                                        return;
                                }
                                if (auto const word = *m_word >> m_index) {
                                        m_index += builtin::ctznz(word);
                                        return;
                                } else {
                                        ++m_word;
                                        m_index = N;
                                }
                                assert(0 < m_index && m_index <= N);
                        } else if constexpr (num_words >= 2) {
                                if (++m_index == N) {
                                        ++m_word;
                                        return;
                                }

                                auto const index = where(m_index);
                                if (index == 0) {
                                        ++m_word;
                                }
                                if (auto const word = *m_word >> index) {
                                        m_index += builtin::ctznz(word);
                                        assert(m_index < N);
                                        return;
                                }
                                ++m_word;

                                for (auto i = which(m_index) + 1; i < num_words; ++i) {
                                        if (auto const word = *m_word) {
                                                m_index = i * word_size + builtin::bsfnz(word);
                                                assert(m_index < N);
                                                return;
                                        }
                                        ++m_word;
                                }
                                m_index = N;
                                assert(0 < m_index && m_index <= N);
                        }
                }

                constexpr auto decrement() // Throws: Nothing.
                {
                        assert(0 < m_index); assert(m_index <= N);
                        if constexpr (num_words == 0) {
                                return;
                        } else if constexpr (num_words == 1) {
                                if (--m_index == 0) {
                                        return;
                                }
                                if (m_index == N - 1) {
                                        --m_word;
                                }
                                if (auto const word = *m_word << (word_size - 1 - m_index)) {
                                        m_index -= builtin::clznz(word);
                                } else {
                                        m_index = 0;
                                }
                                assert(m_index < N);
                        } else if constexpr (num_words >= 2) {
                                if (--m_index == 0) {
                                        return;
                                }

                                auto const index = where(m_index);
                                if (index == word_size - 1 || m_index == N - 1) {
                                        --m_word;
                                }
                                if (auto const word = *m_word << (word_size - 1 - index)) {
                                        m_index -= builtin::clznz(word);
                                        assert(m_index < N);
                                        return;
                                }
                                --m_word;

                                for (auto i = which(m_index) - 1; i >= 0; --i) {
                                        if (auto const word = *m_word) {
                                                m_index = i * word_size + builtin::bsrnz(word);
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

                constexpr auto tied() const noexcept
                {
                        return std::tie(m_word, m_index);
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
        constexpr auto end()           noexcept { using std::end;   return       iterator{end(m_words), N}; }
        constexpr auto end()     const noexcept { using std::end;   return const_iterator{end(m_words), N}; }

        constexpr auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        constexpr auto rend()          noexcept { return       reverse_iterator{begin()}; }
        constexpr auto rend()    const noexcept { return const_reverse_iterator{begin()}; }

        constexpr auto cbegin()  const noexcept { return const_iterator{begin()}; }
        constexpr auto cend()    const noexcept { return const_iterator{end()};   }
        constexpr auto crbegin() const noexcept { return const_reverse_iterator{rbegin()}; }
        constexpr auto crend()   const noexcept { return const_reverse_iterator{rend()};   }

        auto data() const noexcept
        {
                return &m_words[0];
        }

        auto empty() const noexcept
        {
                if constexpr (num_words == 0) {
                        return true;
                } else if constexpr (num_words == 1) {
                        return m_words[0] == mask_none;
                } else if constexpr (num_words >= 2) {
                        using std::cbegin; using std::cend;
                        return std::all_of(cbegin(m_words), cend(m_words), [](auto const word){
                                return word == mask_none;
                        });
                }
        }

        auto full() const noexcept
        {
                if constexpr (excess_bits == 0) {
                        if constexpr (num_words == 0) {
                                return true;
                        } else if constexpr (num_words == 1) {
                                return m_words[0] == mask_all;
                        } else if constexpr (num_words >= 2) {
                                using std::cbegin; using std::cend;
                                return std::all_of(cbegin(m_words), cend(m_words), [](auto const word){
                                        return word == mask_all;
                                });
                        }
                } else {
                        static_assert(num_words != 0);
                        if constexpr (num_words == 1) {
                                return m_words[0] == mask_all >> excess_bits;
                        } else if constexpr (num_words >= 2) {
                                using std::cbegin; using std::cend;
                                return
                                        std::all_of(cbegin(m_words), std::prev(cend(m_words)), [](auto const word){
                                                return word == mask_all;
                                        }) && (m_words[num_words - 1] == mask_all >> excess_bits);
                                ;
                        }
                }
        }

        constexpr auto size() const noexcept
        {
                if constexpr (num_words == 0) {
                        return 0;
                } else if constexpr (num_words == 1) {
                        return builtin::popcount(m_words[0]);
                } else if (num_words >= 2) {
                        // std::accumulate is not constexpr as of C++17
                        auto sum = 0;
                        for (auto const word : m_words) {
                                sum += builtin::popcount(word);
                        }
                        return sum;
                }
        }

        static constexpr auto max_size() noexcept { return N;  }
        static constexpr auto capacity() noexcept { return num_bits; }

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
                /*m_words.*/fill(mask_all);
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
        auto swap(int_set& other) noexcept // (num_words == 0 || is_nothrow_swappable<value_type>{})
        {
                if constexpr (num_words == 1) {
                        using std::swap;
                        swap(m_words[0], other.m_words[0]);
                } else if constexpr (num_words >= 2) {
                        using std::begin; using std::end;
                        std::swap_ranges(begin(m_words), end(m_words), begin(other.m_words));
                }
        }

        auto clear() noexcept
        {
                fill(mask_none);
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
                if constexpr (num_words == 1) {
                        m_words[0] = ~m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto& word : m_words) {
                                word = ~word;
                        }
                }
                sanitize();
                return *this;
        }

        constexpr auto& operator&=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] &= other.m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_words[i] &= other.m_words[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] |= other.m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_words[i] |= other.m_words[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] ^= other.m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_words[i] ^= other.m_words[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] &= ~other.m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_words[i] &= ~other.m_words[i];
                        }
                }
                return *this;
        }

        auto& operator<<=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_words[0] <<= n;
                } else if constexpr (num_words >= 2) {
                        if (n == 0) return *this;

                        auto const n_block = n / word_size;
                        auto const L_shift = n % word_size;

                        using std::begin; using std::end;
                        if (L_shift == 0) {
                                std::copy_backward(begin(m_words), std::prev(end(m_words), n_block), end(m_words));
                        } else {
                                auto const R_shift = word_size - L_shift;

                                for (auto i = num_words - 1; i > n_block; --i) {
                                        m_words[i] =
                                                (m_words[i - n_block    ] << L_shift) |
                                                (m_words[i - n_block - 1] >> R_shift)
                                        ;
                                }
                                m_words[n_block] = m_words[0] << L_shift;
                        }
                        std::fill_n(begin(m_words), n_block, mask_none);
                }
                sanitize();
                return *this;
        }

        auto& operator>>=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_words[0] >>= n;
                } else if constexpr (num_words >= 2) {
                        if (n == 0) return *this;

                        auto const n_block = n / word_size;
                        auto const R_shift = n % word_size;

                        using std::begin; using std::end;
                        if (R_shift == 0) {
                                std::copy_n(std::next(begin(m_words), n_block), num_words - n_block, begin(m_words));
                        } else {
                                auto const L_shift = word_size - R_shift;

                                for (auto i = 0; i < num_words - 1 - n_block; ++i) {
                                        m_words[i] =
                                                (m_words[i + n_block    ] >> R_shift) |
                                                (m_words[i + n_block + 1] << L_shift)
                                        ;
                                }
                                m_words[num_words - 1 - n_block] = m_words[num_words - 1] >> R_shift;
                        }
                        using std::rbegin;
                        std::fill_n(rbegin(m_words), n_block, mask_none);
                }
                return *this;
        }

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction f) const
        {
                if constexpr (num_words == 1) {
                        for (auto word = m_words[0]; word;) {
                                auto const first = builtin::bsfnz(word);
                                f(first);
                                word ^= word_mask(first);
                        }
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0, offset = 0; i < num_words; ++i, offset += word_size) {
                                for (auto word = m_words[i]; word;) {
                                        auto const first = builtin::bsfnz(word);
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
                if constexpr (num_words == 1) {
                        for (auto word = m_words[0]; word;) {
                                auto const last = builtin::bsrnz(word);
                                f(last);
                                word ^= word_mask(last);
                        }
                } else if constexpr (num_words >= 2) {
                        for (auto i = num_words - 1, offset = (size() - 1) * word_size; i >= 0; --i, offset -= word_size) {
                                for (auto word = m_words[i]; word;) {
                                        auto const last = builtin::bsrnz(word);
                                        f(offset + last);
                                        word ^= word_mask(last);
                                }
                        }
                }
                return std::move(f);
        }

private:
        static constexpr auto mask_none =  static_cast<word_type>(0);
        static constexpr auto mask_one  =  static_cast<word_type>(1);
        static constexpr auto mask_all  = ~static_cast<word_type>(0);

        auto fill(word_type const& u) noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] = u;
                } else if constexpr (num_words >= 2) {
                        using std::begin;
                        std::fill_n(begin(m_words), num_words, u);
                }
        }

        static constexpr auto which(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        return 0;
                } else {
                        return n / word_size;
                }
        }

        static constexpr auto where(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        return n;
                } else {
                        return n % word_size;
                }
        }

        static constexpr auto word_mask(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < word_size);
                return mask_one << n;
        }

        constexpr auto sanitize() noexcept
        {
                if constexpr (excess_bits != 0) {
                        m_words[num_words - 1] &= mask_all >> excess_bits;
                }
        }

        friend auto operator==    <>(int_set const&, int_set const&) noexcept;
        friend auto operator<     <>(int_set const&, int_set const&) noexcept;
        friend auto intersects    <>(int_set const&, int_set const&) noexcept;
        friend auto is_subset_of  <>(int_set const&, int_set const&) noexcept;
};

template<int N>
auto operator==(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return true;
        } else if constexpr (num_words == 1) {
                return lhs.m_words[0] == rhs.m_words[0];
        } else if constexpr (num_words >= 2) {
                using std::cbegin; using std::cend;
                return std::equal(cbegin(lhs.m_words), cend(lhs.m_words), cbegin(rhs.m_words), cend(rhs.m_words));
        }
}

template<int N>
auto operator!=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N>
auto operator<(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return false;
        } else if constexpr (num_words == 1) {
                return lhs.m_words[0] < rhs.m_words[0];
        } else if constexpr (num_words >= 2) {
                using std::crbegin; using std::crend;
                return std::lexicographical_compare(crbegin(lhs.m_words), crend(lhs.m_words), crbegin(rhs.m_words), crend(rhs.m_words));
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
constexpr auto operator~(int_set<N> const& lhs) noexcept
{
        auto nrv{lhs}; nrv.flip(); return nrv;
}

template<int N>
constexpr auto operator&(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        auto nrv{lhs}; nrv &= rhs; return nrv;
}

template<int N>
constexpr auto operator|(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        auto nrv{lhs}; nrv |= rhs; return nrv;
}

template<int N>
constexpr auto operator^(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        auto nrv{lhs}; nrv ^= rhs; return nrv;
}

template<int N>
constexpr auto operator-(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        auto nrv{lhs}; nrv -= rhs; return nrv;
}

template<int N>
auto operator<<(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv <<= n; return nrv;
}

template<int N>
auto operator>>(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv >>= n; return nrv;
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
auto intersects(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return false;
        } else if constexpr (num_words == 1) {
                return lhs.m_words[0] & rhs.m_words[0];
        } else if constexpr (num_words >= 2) {
                using std::cbegin; using std::cend;
                return !std::equal(cbegin(lhs.m_words), cend(lhs.m_words), cbegin(rhs.m_words), cend(rhs.m_words), [](auto const w1, auto const w2) {
                        return !(w1 & w2);
                });
        }
}

template<int N>
auto disjoint(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<int N>
auto is_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        static constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return true;
        } else if constexpr (num_words == 1) {
                return !(lhs.m_words[0] & ~rhs.m_words[0]);
        } else if constexpr (num_words >= 2) {
                using std::cbegin; using std::cend;
                return std::equal(cbegin(lhs.m_words), cend(lhs.m_words), cbegin(rhs.m_words), cend(rhs.m_words), [](auto const w1, auto const w2) {
                        return !(w1 & ~w2);
                });
        }
}

template<int N>
auto is_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<int N>
auto is_proper_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<int N>
auto is_proper_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
}

template<class HashAlgorithm, int N>
auto hash_append(HashAlgorithm& h, int_set<N> const& is)
{
        h(is.data(), is.capacity() /  CHAR_BIT);
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
constexpr auto data(int_set<N> const& is)
        -> decltype(is.data())
{
        return is.data();
}

}       // namespace xstd
