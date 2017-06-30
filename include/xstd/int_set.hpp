#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // all_of, copy_backward, copy_n, equal, fill_n, lexicographical_compare
#include <array>                // array
#include <cassert>              // assert
#include <cstddef>              // size_t
#include <cstdint>              // uint64_t
#include <functional>           // less
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, reverse_iterator
#include <limits>               // digits
#include <tuple>                // tie
#include <type_traits>          // conditional_t, is_integral_v, is_nothrow_swappable_v, is_pod_v, is_unsigned_v
#include <utility>              // move

#define PP_STL_CONSTEXPR_INCOMPLETE

namespace xstd {
namespace builtin {
namespace detail {

// GCC / Clang have support for extended 128-bit integers.
// Uset get<0> and get<1> to extract the lower and upper 64-bit integers.

template<int N>
constexpr auto get(__uint128_t x) noexcept
{
        static_assert(0 <= N); static_assert(N < 2);
        return static_cast<uint64_t>(x >> (N * 64));
}

// GCC / Clang have built-in functions for Count Trailing Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined.

struct ctznz
{
        constexpr auto operator()(unsigned x) const // Throws: Nothing.
        {
                return __builtin_ctz(x);
        }

        constexpr auto operator()(unsigned long x) const // Throws: Nothing.
        {
                return __builtin_ctzl(x);
        }

        constexpr auto operator()(unsigned long long x) const // Throws: Nothing.
        {
                return __builtin_ctzll(x);
        }

        constexpr auto operator()(__uint128_t x) const // Throws: Nothing.
        {
                if (auto const lower = get<0>(x); lower) {
                        return __builtin_ctzll(lower);
                } else {
                        return __builtin_ctzll(get<1>(x)) + 64;
                }
        }
};

// GCC / Clang have built-in functions for Count Leading Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined

struct clznz
{
        constexpr auto operator()(unsigned x) const // Throws: Nothing.
        {
                return __builtin_clz(x);
        }

        constexpr auto operator()(unsigned long x) const // Throws: Nothing.
        {
                return __builtin_clzl(x);
        }

        constexpr auto operator()(unsigned long long x) const // Throws: Nothing.
        {
                return __builtin_clzll(x);
        }

        constexpr auto operator()(__uint128_t x) const // Throws: Nothing.
        {
                if (auto const upper = get<1>(x)) {
                        return __builtin_clzll(upper);
                } else {
                        return __builtin_clzll(get<0>(x)) + 64;
                }
        }
};

// GCC / Clang have built-in functions for Population Count
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

template<class WordT>
constexpr auto ctznz(WordT x) // Throws: Nothing.
{
        assert(x != 0);
        return detail::ctznz{}(x);
}

template<class WordT>
constexpr auto clznz(WordT x) // Throws: Nothing.
{
        assert(x != 0);
        return detail::clznz{}(x);
}

template<class WordT>
constexpr auto popcount(WordT x) noexcept
{
        return detail::popcount{}(x);
}

template<class WordT>
constexpr auto bsfnz(WordT x) // Throws: Nothing.
{
        assert(x != 0);
        return ctznz(x);
}

template<class WordT>
constexpr auto bsrnz(WordT x) // Throws: Nothing.
{
        assert(x != 0);
        return std::numeric_limits<WordT>::digits - 1 - clznz(x);
}

template<class WordT>
constexpr auto ctz(WordT x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<WordT>::digits;
}

template<class WordT>
constexpr auto clz(WordT x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<WordT>::digits;
}

template<class WordT>
constexpr auto bsf(WordT x) noexcept
{
        return ctz(x);
}

template<class WordT>
constexpr auto bsr(WordT x) noexcept
{
        return std::numeric_limits<WordT>::digits - 1 - clz(x);
}

}       // namespace builtin

template<int>
class int_set;

template<int N> PP_STL_CONSTEXPR_INCOMPLETE auto operator==  (int_set<N> const& /* lhs */, int_set<N> const& /* rhs */) noexcept;
template<int N> PP_STL_CONSTEXPR_INCOMPLETE auto operator<   (int_set<N> const& /* lhs */, int_set<N> const& /* rhs */) noexcept;
template<int N> PP_STL_CONSTEXPR_INCOMPLETE auto intersects  (int_set<N> const& /* lhs */, int_set<N> const& /* rhs */) noexcept;
template<int N> PP_STL_CONSTEXPR_INCOMPLETE auto is_subset_of(int_set<N> const& /* lhs */, int_set<N> const& /* rhs */) noexcept;

template<int N>
class int_set
{
        static_assert(0 <= N);

        constexpr static auto static_assert_type_traits() noexcept
        {
                static_assert(std::is_pod_v<int_set>);
        }

        using word_type = __uint128_t;//std::conditional_t<N <= 64, uint64_t, __uint128_t>;
        static_assert(std::is_unsigned_v<word_type>);
        static_assert(std::is_integral_v<word_type>);

        constexpr static auto word_size = std::numeric_limits<word_type>::digits;
        constexpr static auto num_words = (N - 1 + word_size) / word_size;
        constexpr static auto num_bits = num_words * word_size;
        constexpr static auto excess_bits = num_bits - N;

        std::array<word_type, num_words> m_words;
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
                        assert(0 <= m_value); assert(m_value < N);
                }

                word_type const& m_word;
                value_type const m_value;

        public:
                ~const_reference() = default;
                const_reference(const_reference const&) = default;
                const_reference(const_reference&&) = default;
                const_reference& operator=(const_reference const&) = delete;
                const_reference& operator=(const_reference&&) = delete;

                const_reference() = delete;

                constexpr const_reference(word_type const& w, value_type const v) noexcept
                :
                        m_word{w},
                        m_value{v}
                {
                        assert_invariants();
                }

                const_reference& operator=(value_type const) = delete;

                /* implicit */ constexpr operator value_type() const noexcept
                {
                        return m_value;
                }

                constexpr auto operator&() const noexcept
                        -> const_iterator
                {
                        return { &m_word, m_value };
                }
        };

        class const_iterator
        {
                constexpr auto assert_invariants() const noexcept
                {
                        if constexpr (num_words > 0) {
                                assert(m_word != nullptr);
                        }
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
                        m_index{0}
                {
                        find_first();
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
                        return lhs.m_index == rhs.m_index;
                }

                friend constexpr auto operator!=(const_iterator lhs, const_iterator rhs) noexcept
                {
                        return not (lhs == rhs);
                }
        private:
                constexpr auto find_first() noexcept
                {
                        assert(0 == m_index);
                        if constexpr (excess_bits == 0) {
                                if constexpr (num_words == 1) {
                                        if (auto const word = *m_word; word) {
                                                m_index += builtin::ctznz(word);
                                                assert(0 <= m_index); assert(m_index < N);
                                                return;
                                        }
                                        ++m_word;
                                        m_index += word_size;
                                } else if constexpr (num_words >= 2) {
                                        for (auto i = 0; i < num_words; ++i, ++m_word, m_index += word_size) {
                                                if (auto const word = *m_word; word) {
                                                        m_index += builtin::ctznz(word);
                                                        assert(i * word_size <= m_index); assert(m_index < N);
                                                        return;
                                                }
                                        }
                                }
                        } else {
                                static_assert(num_words > 0);
                                if constexpr (num_words >= 2) {
                                        for (auto i = 0; i < num_words - 1; ++i, ++m_word, m_index += word_size) {
                                                if (auto const word = *m_word; word) {
                                                        m_index += builtin::ctznz(word);
                                                        assert(i * word_size <= m_index); assert(m_index < N);
                                                        return;
                                                }
                                        }
                                }
                                if (auto const word = *m_word; word) {
                                        m_index += builtin::ctznz(word);
                                        assert((num_words - 1) * word_size <= m_index); assert(m_index < N);
                                        return;
                                }
                                ++m_word;
                                m_index = N;
                        }
                        assert(m_index == N);
                }

                constexpr auto increment() // Throws: Nothing.
                {
                        assert(0 <= m_index); assert(m_index < N);
                        if constexpr (num_words == 1) {
                                if (++m_index == N) {
                                        ++m_word;
                                        return;
                                }
                                if (auto const word = *m_word >> m_index; word) {
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
                                if (auto const word = *m_word >> index; word) {
                                        m_index += builtin::ctznz(word);
                                        assert(m_index < N);
                                        return;
                                }
                                ++m_word;

                                for (auto i = which(m_index) + 1; i < num_words; ++i) {
                                        if (auto const word = *m_word; word) {
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
                        if constexpr (num_words == 1) {
                                if (--m_index == 0) {
                                        return;
                                }
                                if (m_index == N - 1) {
                                        --m_word;
                                }
                                if (auto const word = *m_word << (word_size - 1 - m_index)) {
                                        m_index -= builtin::clznz(word);
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
                                }
                        }
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

        constexpr auto& operator=(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                insert(ilist);
                return *this;
        }

        constexpr auto begin()         noexcept { return       iterator{m_words.begin()}; }
        constexpr auto begin()   const noexcept { return const_iterator{m_words.begin()}; }
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

        constexpr auto data() const noexcept
        {
                return m_words.data();
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto empty() const noexcept
        {
                if constexpr (num_words == 0) {
                        return true;
                } else if constexpr (num_words == 1) {
                        return m_words[0] == mask_none;
                } else if constexpr (num_words >= 2) {
                        return std::all_of(m_words.cbegin(), m_words.cend(), [](auto const word) {
                                return word == mask_none;
                        });
                }
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto full() const noexcept
        {
                if constexpr (excess_bits == 0) {
                        if constexpr (num_words == 0) {
                                return true;
                        } else if constexpr (num_words == 1) {
                                return m_words[0] == mask_all;
                        } else if constexpr (num_words >= 2) {
                                return std::all_of(m_words.cbegin(), m_words.cend(), [](auto const word) {
                                        return word == mask_all;
                                });
                        }
                } else {
                        static_assert(num_words > 0);
                        if constexpr (num_words == 1) {
                                return m_words[0] == mask_sane;
                        } else if constexpr (num_words >= 2) {
                                return
                                        std::all_of(m_words.cbegin(), m_words.cend() - 1, [](auto const word) {
                                                return word == mask_all;
                                        }) && (m_words.back() == mask_sane);
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
                        for (auto&& word : m_words) {
                                sum += builtin::popcount(word);
                        }
                        return sum;
                }
        }

        constexpr static auto max_size() noexcept { return N;  }
        constexpr static auto capacity() noexcept { return num_bits; }

        constexpr auto& insert(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[static_cast<std::size_t>(which(n))] |= word_mask(where(n));
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

        PP_STL_CONSTEXPR_INCOMPLETE auto& fill() noexcept
        {
                m_words.fill(mask_all);
                sanitize_back();
                assert(full());
                return *this;
        }

        constexpr auto& erase(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[static_cast<std::size_t>(which(n))] &= ~word_mask(where(n));
                assert(not test(n));
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

        PP_STL_CONSTEXPR_INCOMPLETE auto swap(int_set& other) noexcept(num_words == 0 || std::is_nothrow_swappable_v<value_type>)
        {
                m_words.swap(other.m_words);
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto clear() noexcept
        {
                m_words.fill(mask_none);
                assert(empty());
        }

        constexpr auto& flip(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                m_words[static_cast<std::size_t>(which(n))] ^= word_mask(where(n));
                return *this;
        }

        constexpr auto test(value_type const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                return (m_words[static_cast<std::size_t>(which(n))] & word_mask(where(n))) != mask_none;
        }

        constexpr auto& flip() noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] = ~m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto&& word : m_words) {
                                word = ~word;
                        }
                }
                sanitize_back();
                return *this;
        }

        constexpr auto& operator&=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_words[0] &= other.m_words[0];
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_words[static_cast<std::size_t>(i)] &= other.m_words[static_cast<std::size_t>(i)];
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
                                m_words[static_cast<std::size_t>(i)] |= other.m_words[static_cast<std::size_t>(i)];
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
                                m_words[static_cast<std::size_t>(i)] ^= other.m_words[static_cast<std::size_t>(i)];
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
                                m_words[static_cast<std::size_t>(i)] &= ~other.m_words[static_cast<std::size_t>(i)];
                        }
                }
                return *this;
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto& operator<<=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_words[0] <<= n;
                } else if constexpr (num_words >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / word_size;
                        auto const L_shift = n % word_size;

                        if (L_shift == 0) {
                                std::copy_backward(m_words.begin(), m_words.end() - n_block, m_words.end());
                        } else {
                                auto const R_shift = word_size - L_shift;

                                for (auto i = num_words - 1; i > n_block; --i) {
                                        m_words[static_cast<std::size_t>(i)] =
                                                (m_words[static_cast<std::size_t>(i - n_block    )] << L_shift) |
                                                (m_words[static_cast<std::size_t>(i - n_block - 1)] >> R_shift)
                                        ;
                                }
                                m_words[static_cast<std::size_t>(n_block)] = m_words.front() << L_shift;
                        }
                        std::fill_n(m_words.begin(), n_block, mask_none);
                }
                sanitize_back();
                return *this;
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto& operator>>=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_words[0] >>= n;
                } else if constexpr (num_words >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / word_size;
                        auto const R_shift = n % word_size;

                        if (R_shift == 0) {
                                std::copy_n(m_words.begin() + n_block, num_words - n_block, m_words.begin());
                        } else {
                                auto const L_shift = word_size - R_shift;

                                for (auto i = 0; i < num_words - 1 - n_block; ++i) {
                                        m_words[static_cast<std::size_t>(i)] =
                                                (m_words[static_cast<std::size_t>(i + n_block    )] >> R_shift) |
                                                (m_words[static_cast<std::size_t>(i + n_block + 1)] << L_shift)
                                        ;
                                }
                                m_words[static_cast<std::size_t>(num_words - 1 - n_block)] = m_words.back() >> R_shift;
                        }
                        std::fill_n(m_words.rbegin(), n_block, mask_none);
                }
                return *this;
        }

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction fun) const
        {
                if constexpr (num_words == 1) {
                        for (auto word = m_words[0]; word; /* update inside loop */) {
                                auto const first = builtin::bsfnz(word);
                                fun(first);
                                word ^= word_mask(first);
                        }
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0, offset = 0; i < num_words; ++i, offset += word_size) {
                                for (auto word = m_words[static_cast<std::size_t>(i)]; word; /* update inside loop */) {
                                        auto const first = builtin::bsfnz(word);
                                        fun(offset + first);
                                        word ^= word_mask(first);
                                }
                        }
                }
                return std::move(fun);
        }

        template<class UnaryFunction>
        constexpr auto reverse_for_each(UnaryFunction fun) const
        {
                if constexpr (num_words == 1) {
                        for (auto word = m_words[0]; word; /* update inside loop */) {
                                auto const last = builtin::bsrnz(word);
                                fun(last);
                                word ^= word_mask(last);
                        }
                } else if constexpr (num_words >= 2) {
                        for (auto i = num_words - 1, offset = (size() - 1) * word_size; i >= 0; --i, offset -= word_size) {
                                for (auto word = m_words[static_cast<std::size_t>(i)]; word; /* update inside loop */) {
                                        auto const last = builtin::bsrnz(word);
                                        fun(offset + last);
                                        word ^= word_mask(last);
                                }
                        }
                }
                return std::move(fun);
        }

private:
        constexpr static auto mask_none =  static_cast<word_type>(0);
        constexpr static auto mask_all  = ~static_cast<word_type>(0);
        constexpr static auto mask_sane = mask_all >> excess_bits;

        constexpr auto sanitize_back() noexcept
        {
                if constexpr (excess_bits != 0) {
                        m_words.back() &= mask_sane;
                }
        }

        constexpr static auto which(value_type const n [[maybe_unused]]) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        return 0;
                } else {
                        return n / word_size;
                }
        }

        constexpr static auto where(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        return n;
                } else {
                        return n % word_size;
                }
        }

        constexpr static auto word_mask_table = []() {
                auto table = std::array<word_type, word_size>{};
                for (auto n = 0; n < word_size; ++n) {
                        table[static_cast<std::size_t>(n)] = static_cast<word_type>(1) << n;
                }
                return table;
        }();

        constexpr static auto word_mask(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < word_size);
                return word_mask_table[static_cast<std::size_t>(n)];
        }

        friend PP_STL_CONSTEXPR_INCOMPLETE auto operator==    <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend PP_STL_CONSTEXPR_INCOMPLETE auto operator<     <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend PP_STL_CONSTEXPR_INCOMPLETE auto intersects    <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend PP_STL_CONSTEXPR_INCOMPLETE auto is_subset_of  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
};

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator==(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return true;
        } else if constexpr (num_words == 1) {
                return lhs.m_words[0] == rhs.m_words[0];
        } else if constexpr (num_words >= 2) {
                return lhs.m_words == rhs.m_words;
        }
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator!=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return not (lhs == rhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator<(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return false;
        } else if constexpr (num_words == 1) {
                return lhs.m_words[0] < rhs.m_words[0];
        } else if constexpr (num_words >= 2) {
                return std::lexicographical_compare(
                        lhs.m_words.crbegin(), lhs.m_words.crend(),
                        rhs.m_words.crbegin(), rhs.m_words.crend()
                );
        }
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator>(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return rhs < lhs;
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator>=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return not (lhs < rhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator<=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return not (rhs < lhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto swap(int_set<N>& lhs, int_set<N>& rhs) noexcept(noexcept(lhs.swap(rhs)))
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
PP_STL_CONSTEXPR_INCOMPLETE auto operator<<(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv <<= n; return nrv;
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator>>(int_set<N> const& lhs, int const n) // Throws: Nothing.
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
PP_STL_CONSTEXPR_INCOMPLETE auto set_transform_plus(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        return lhs << n;
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto set_transform_minus(int_set<N> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        return lhs >> n;
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto intersects(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr static auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return false;
        } else if constexpr (num_words == 1) {
                return lhs.m_words[0] & rhs.m_words[0];
        } else if constexpr (num_words >= 2) {
                return not std::equal(
                        lhs.m_words.cbegin(), lhs.m_words.cend(),
                        rhs.m_words.cbegin(), rhs.m_words.cend(),
                        [](auto const wL, auto const wR) {
                                return not (wL & wR);
                        }
                );
        }
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto disjoint(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return not intersects(lhs, rhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto is_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr static auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return true;
        } else if constexpr (num_words == 1) {
                return not (lhs.m_words[0] & ~rhs.m_words[0]);
        } else if constexpr (num_words >= 2) {
                return std::equal(
                        lhs.m_words.cbegin(), lhs.m_words.cend(),
                        rhs.m_words.cbegin(), rhs.m_words.cend(),
                        [](auto const wL, auto const wR) {
                                return not (wL & ~wR);
                        }
                );
        }
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto is_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto is_proper_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && not is_subset_of(rhs, lhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto is_proper_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && not is_superset_of(rhs, lhs);
}

template<class HashAlgorithm, int N>
auto hash_append(HashAlgorithm& h, int_set<N> const& is)
{
        h(is.data(), is.capacity() /  std::numeric_limits<unsigned char>::digits);
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

#undef PP_STL_CONSTEXPR_INCOMPLETE
