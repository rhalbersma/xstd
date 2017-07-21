#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // all_of, copy_backward, copy_n, equal, fill_n, lexicographical_compare, max, swap_ranges
#include <array>                // array
#include <cassert>              // assert
#include <cstddef>              // size_t
#include <cstdint>              // uint64_t
#include <functional>           // less
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, crbegin, crend, rbegin, reverse_iterator
#include <limits>               // digits
#include <type_traits>          // conditional_t, is_integral_v, is_nothrow_swappable_v, is_pod_v, is_unsigned_v
#include <utility>              // move, swap

#define PP_STL_CONSTEXPR_INCOMPLETE

namespace xstd {
namespace detail {

// GCC / Clang have support for extended 128-bit integers.
// Uset get<0> and get<1> to extract the lower and upper 64-bit integers.

template<int N>
constexpr auto get(__uint128_t x) noexcept
{
        static_assert(0 <= N); static_assert(N < 2);
        return static_cast<uint64_t>(x >> (N * 64));
}

template<class UIntType> constexpr auto zero =  static_cast<UIntType>(0);
template<class UIntType> constexpr auto ones = ~static_cast<UIntType>(0);

namespace builtin {

// GCC / Clang have built-in functions for Count Trailing Zeros
// for unsigned, unsigned long and unsigned long long.
// For zero input, the result is undefined.

struct ctznz
{
        constexpr auto operator()(unsigned x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_ctz(x);
        }

        constexpr auto operator()(unsigned long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_ctzl(x);
        }

        constexpr auto operator()(unsigned long long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_ctzll(x);
        }

        constexpr auto operator()(__uint128_t x) const // Throws: Nothing.
        {
                assert(x != 0);
                if (auto const lower = get<0>(x); lower != detail::zero<uint64_t>) {
                        return ctznz{}(lower);
                } else {
                        return ctznz{}(get<1>(x)) + 64;
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
                assert(x != 0);
                return __builtin_clz(x);
        }

        constexpr auto operator()(unsigned long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_clzl(x);
        }

        constexpr auto operator()(unsigned long long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_clzll(x);
        }

        constexpr auto operator()(__uint128_t x) const // Throws: Nothing.
        {
                assert(x != 0);
                if (auto const upper = get<1>(x); upper != detail::zero<uint64_t>) {
                        return clznz{}(upper);
                } else {
                        return clznz{}(get<0>(x)) + 64;
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
                return popcount{}(get<0>(x)) + popcount{}(get<1>(x));
        }
};

}       // namespace builtin

template<class UIntType>
constexpr auto ctznz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::ctznz{}(x);
}

template<class UIntType>
constexpr auto clznz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::clznz{}(x);
}

template<class UIntType>
constexpr auto popcount(UIntType x) noexcept
{
        return builtin::popcount{}(x);
}

template<class UIntType>
constexpr auto bsfnz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return ctznz(x);
}

template<class UIntType>
constexpr auto bsrnz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return std::numeric_limits<UIntType>::digits - 1 - clznz(x);
}

template<class UIntType>
constexpr auto ctz(UIntType x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<UIntType>::digits;
}

template<class UIntType>
constexpr auto clz(UIntType x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<UIntType>::digits;
}

template<class UIntType>
constexpr auto bsf(UIntType x) noexcept
{
        return ctz(x);
}

template<class UIntType>
constexpr auto bsr(UIntType x) noexcept
{
        return std::numeric_limits<UIntType>::digits - 1 - clz(x);
}

template<class UIntType>
constexpr auto bit1_table = []() {
        constexpr auto N = std::numeric_limits<UIntType>::digits;
        auto table = std::array<UIntType, N>{};
        for (auto n = 0; n < N; ++n) {
                table[static_cast<std::size_t>(n)] = static_cast<UIntType>(1) << n;
        }
        return table;
}();

template<class UIntType>
constexpr auto bit1(int n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < std::numeric_limits<UIntType>::digits);
        return bit1_table<UIntType>[static_cast<std::size_t>(n)];
}

}       // namespace detail

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

        using word_type = std::conditional_t<N <= 64, uint64_t, __uint128_t>;
        static_assert(std::is_unsigned_v<word_type>);
        static_assert(std::is_integral_v<word_type>);

        constexpr static auto word_size = std::numeric_limits<word_type>::digits;
        constexpr static auto num_words = (N - 1 + word_size) / word_size;
        constexpr static auto num_bits = num_words * word_size;
        constexpr static auto excess_bits = num_bits - N;

        using data_type = std::conditional_t<num_words == 1, word_type, word_type[std::max(num_words, 1)]>;
        data_type m_data;
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
        public:
                using difference_type   = int_set::difference_type;
                using value_type        = int_set::value_type;
                using pointer           = const_iterator;
                using reference         = const_reference;
                using iterator_category = std::bidirectional_iterator_tag;

        private:
                constexpr auto assert_invariants() const noexcept
                {
                        assert(m_word != nullptr);
                        assert(0 <= m_value); assert(m_value < N || num_bits == m_value);
                }

                word_type const* m_word;
                value_type m_value;

        public:
                const_iterator() = default;

                explicit constexpr const_iterator(word_type const* w) // Throws: Nothing.
                :
                        m_word{w},
                        m_value{find_first()}
                {
                        assert_invariants();
                }

                constexpr const_iterator(word_type const* w, value_type const v) // Throws: Nothing.
                :
                        m_word{w},
                        m_value{v}
                {
                        assert_invariants();
                }

                constexpr auto operator*() const // Throws: Nothing.
                        -> const_reference
                {
                        assert(0 <= m_value); assert(m_value < N);
                        return { *m_word, m_value };
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
                        assert(lhs.m_word == rhs.m_word);
                        return lhs.m_value == rhs.m_value;
                }

                friend constexpr auto operator!=(const_iterator lhs, const_iterator rhs) noexcept
                {
                        return !(lhs == rhs);
                }

        private:
                constexpr auto find_first() noexcept
                {
                        if constexpr (num_words == 0) {
                                return 0;
                        } else if constexpr (num_words == 1) {
                                return detail::ctz(*m_word);
                        } else if constexpr (num_words >= 2) {
                                auto offset = 0;
                                for (auto i = 0; i < num_words; ++i, offset += word_size) {
                                        if (auto const word = m_word[i]; word != detail::zero<word_type>) {
                                                offset += detail::ctznz(word);
                                                break;
                                        }
                                }
                                return offset;
                        }
                }

                constexpr auto increment() // Throws: Nothing.
                {
                        assert(m_value < N);
                        if (num_bits == ++m_value) { return; }
                        if constexpr (num_words == 1) {
                                if (auto const word = *m_word >> m_value; word != detail::zero<word_type>) {
                                        m_value += detail::ctznz(word);
                                        return;
                                }
                                m_value = word_size;
                        } else if constexpr (num_words >= 2) {
                                auto i = which(m_value);
                                if (auto const offset = where(m_value); offset != 0) {
                                        if (auto const word = m_word[i] >> offset; word != detail::zero<word_type>) {
                                                m_value += detail::ctznz(word);
                                                return;
                                        }
                                        ++i;
                                        m_value += word_size - offset;
                                }
                                for (/* initialized before loop */; i < num_words; ++i, m_value += word_size) {
                                        if (auto const word = m_word[i]; word != detail::zero<word_type>) {
                                                m_value += detail::ctznz(word);
                                                return;
                                        }
                                }
                        }
                        assert(num_bits == m_value);
                }

                constexpr auto decrement() // Throws: Nothing.
                {
                        assert(0 < m_value);
                        --m_value;
                        if constexpr (num_words == 1) {
                                m_value -= detail::clznz(*m_word << (word_size - 1 - m_value));
                        } else if constexpr (num_words >= 2) {
                                auto i = which(m_value);
                                if (auto const offset = where(m_value); offset != word_size - 1) {
                                        if (auto const word = m_word[i] << (word_size - 1 - offset); word != detail::zero<word_type>) {
                                                m_value -= detail::clznz(word);
                                                return;
                                        }
                                        --i;
                                        m_value -= offset + 1;
                                }
                                for (/* initialized before loop */; i >= 0; --i, m_value -= word_size) {
                                        if (auto const word = m_word[i]; word != detail::zero<word_type>) {
                                                m_value -= detail::clznz(word);
                                                return;
                                        }
                                }
                        }
                        assert(0 <= m_value); assert(m_value < N);
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
                m_data{}
        {
                insert(first, last);
        }

        constexpr int_set(std::initializer_list<value_type> ilist) // Throws: Nothing.
        :
                int_set(ilist.begin(), ilist.end())
        {}

        constexpr auto& operator=(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                insert(ilist.begin(), ilist.end());
                return *this;
        }

        constexpr auto begin()         noexcept { return       iterator{data()}; }
        constexpr auto begin()   const noexcept { return const_iterator{data()}; }
        constexpr auto end()           noexcept { return       iterator{data(), num_bits}; }
        constexpr auto end()     const noexcept { return const_iterator{data(), num_bits}; }

        constexpr auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        constexpr auto rend()          noexcept { return       reverse_iterator{begin()}; }
        constexpr auto rend()    const noexcept { return const_reverse_iterator{begin()}; }

        constexpr auto cbegin()  const noexcept { return const_iterator{begin()}; }
        constexpr auto cend()    const noexcept { return const_iterator{end()};   }
        constexpr auto crbegin() const noexcept { return const_reverse_iterator{rbegin()}; }
        constexpr auto crend()   const noexcept { return const_reverse_iterator{rend()};   }

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction fun) const
        {
                if constexpr (num_words == 1) {
                        for (auto word = m_data; word; /* update inside loop */) {
                                auto const first = detail::bsfnz(word);
                                fun(first);
                                word ^= detail::bit1<word_type>(first);
                        }
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0, offset = 0; i < num_words; ++i, offset += word_size) {
                                for (auto word = m_data[i]; word; /* update inside loop */) {
                                        auto const first = detail::bsfnz(word);
                                        fun(offset + first);
                                        word ^= detail::bit1<word_type>(first);
                                }
                        }
                }
                return std::move(fun);
        }

        template<class UnaryFunction>
        constexpr auto reverse_for_each(UnaryFunction fun) const
        {
                if constexpr (num_words == 1) {
                        for (auto word = m_data; word; /* update inside loop */) {
                                auto const last = detail::bsrnz(word);
                                fun(last);
                                word ^= detail::bit1<word_type>(last);
                        }
                } else if constexpr (num_words >= 2) {
                        for (auto i = num_words - 1, offset = (num_words - 1) * word_size; i >= 0; --i, offset -= word_size) {
                                for (auto word = m_data[i]; word; /* update inside loop */) {
                                        auto const last = detail::bsrnz(word);
                                        fun(offset + last);
                                        word ^= detail::bit1<word_type>(last);
                                }
                        }
                }
                return std::move(fun);
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto empty() const noexcept
        {
                if constexpr (num_words == 0) {
                        return true;
                } else if constexpr (num_words == 1) {
                        return m_data == detail::zero<word_type>;
                } else if constexpr (num_words >= 2) {
                        return std::all_of(m_data, m_data + num_words, [](auto const word) {
                                return word == detail::zero<word_type>;
                        });
                }
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto full() const noexcept
        {
                if constexpr (excess_bits == 0) {
                        if constexpr (num_words == 0) {
                                return true;
                        } else if constexpr (num_words == 1) {
                                return m_data == detail::ones<word_type>;
                        } else if constexpr (num_words >= 2) {
                                return std::all_of(m_data, m_data + num_words, [](auto const word) {
                                        return word == detail::ones<word_type>;
                                });
                        }
                } else {
                        if constexpr (num_words == 1) {
                                return m_data == sane;
                        } else {
                                static_assert(num_words >= 2);
                                return
                                        std::all_of(m_data, m_data + num_words - 1, [](auto const word) {
                                                return word == detail::ones<word_type>;
                                        }) && m_data[num_words - 1] == sane;
                                ;
                        }
                }
        }

        constexpr auto count() const noexcept
        {
                if constexpr (num_words == 0) {
                        return 0;
                } else if constexpr (num_words == 1) {
                        return detail::popcount(m_data);
                } else if (num_words >= 2) {
                        // std::accumulate is not constexpr as of C++17
                        auto sum = 0;
                        for (auto&& word : m_data) {
                                sum += detail::popcount(word);
                        }
                        return sum;
                }
        }

        constexpr static auto max_size() noexcept { return N; }
        constexpr static auto capacity() noexcept { return num_bits; }

        [[deprecated]] auto none() const noexcept { return empty(); }
        [[deprecated]] auto any() const noexcept { return !empty(); }
        [[deprecated]] auto all() const noexcept { return full(); }
        [[deprecated]] constexpr auto size() const noexcept { return max_size(); }

        constexpr auto& insert(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_data |= detail::bit1<word_type>(n);
                } else {
                        m_data[which(n)] |= detail::bit1<word_type>(where(n));
                }
                assert(contains(n));
                return *this;
        }

        template<class InputIterator>
        constexpr auto insert(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                while (first != last) {
                        insert(*first++);
                }
        }

        constexpr auto insert(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                insert(ilist.begin(), ilist.end());
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto& fill() noexcept
        {
                if constexpr (num_words == 1) {
                        m_data = detail::ones<word_type>;
                } else if constexpr (num_words >= 2) {
                        std::fill_n(m_data, num_words, detail::ones<word_type>);
                }
                sanitize_back();
                assert(full());
                return *this;
        }

        constexpr auto& erase(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_data &= ~detail::bit1<word_type>(n);
                } else {
                        m_data[which(n)] &= ~detail::bit1<word_type>(where(n));
                }
                assert(!contains(n));
                return *this;
        }

        constexpr auto& erase(const_iterator it) // Throws: Nothing.
        {
                return erase(*it);
        }

        template<class InputIterator>
        constexpr auto erase(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                while (first != last) {
                        erase(*first++);
                }
        }

        constexpr auto erase(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                erase(ilist.begin(), ilist.end());
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto swap(int_set& other) noexcept(num_words == 0 || std::is_nothrow_swappable_v<value_type>)
        {
                if constexpr (num_words == 1) {
                        using std::swap;
                        swap(m_data, other.m_data);
                } else if constexpr (num_words >= 2) {
                        std::swap_ranges(m_data, m_data + num_words, other.m_data);
                }
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto clear() noexcept
        {
                if constexpr (num_words == 1) {
                        m_data = detail::zero<word_type>;
                } else if constexpr (num_words >= 2) {
                        std::fill_n(m_data, num_words, detail::zero<word_type>);
                }
                assert(empty());
        }

        constexpr auto& toggle(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_data ^= detail::bit1<word_type>(n);
                } else {
                        m_data[which(n)] ^= detail::bit1<word_type>(where(n));
                }
                return *this;
        }

        constexpr auto& toggle() noexcept
        {
                if constexpr (num_words == 1) {
                        m_data = ~m_data;
                } else if constexpr (num_words >= 2) {
                        for (auto&& word : m_data) {
                                word = ~word;
                        }
                }
                sanitize_back();
                return *this;
        }

        constexpr auto contains(value_type const n) const // Throws: Nothing.
                -> bool
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        return m_data & detail::bit1<word_type>(n);
                } else {
                        return m_data[which(n)] & detail::bit1<word_type>(where(n));
                }
        }

        [[deprecated]] auto& set(std::size_t pos, bool val = true)
        {
                if (pos > static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N>::set(): index out of range"};
                }
                return val ? insert(static_cast<int>(pos)) : erase(static_cast<int>(pos));
        }

        [[deprecated]] auto& set() noexcept
        {
                return fill();
        }

        [[deprecated]] auto& reset(std::size_t pos, bool val = true)
        {
                if (pos > static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N>::reset(): index out of range"};
                }
                return val ? erase(static_cast<int>(pos)) : insert(static_cast<int>(pos));
        }

        [[deprecated]] auto& reset() noexcept
        {
                return clear();
        }

        [[deprecated]] auto& flip(std::size_t pos)
        {
                if (pos > static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N>::flip(): index out of range"};
                }
                return toggle(static_cast<int>(pos));
        }

        [[deprecated]] auto& flip() noexcept
        {
                return toggle();
        }

        [[deprecated]] constexpr auto operator[](std::size_t pos) const // Throws: Nothing.
        {
                return contains(static_cast<int>(pos));
        }

        [[deprecated]] auto test(std::size_t pos) const
        {
                if (pos > static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N>::test(): index out of range"};
                }
                return contains(static_cast<int>(pos));
        }

        constexpr auto& operator&=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_data &= other.m_data;
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_data[i] &= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_data |= other.m_data;
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_data[i] |= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_data ^= other.m_data;
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_data[i] ^= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(int_set const& other) noexcept
        {
                if constexpr (num_words == 1) {
                        m_data &= ~other.m_data;
                } else if constexpr (num_words >= 2) {
                        for (auto i = 0; i < num_words; ++i) {
                                m_data[i] &= ~other.m_data[i];
                        }
                }
                return *this;
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto& operator<<=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_data <<= n;
                } else if constexpr (num_words >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / word_size;
                        auto const L_shift = n % word_size;

                        if (L_shift == 0) {
                                std::copy_backward(m_data, m_data + num_words - n_block, m_data + num_words);
                        } else {
                                auto const R_shift = word_size - L_shift;

                                for (auto i = num_words - 1; i > n_block; --i) {
                                        m_data[i] =
                                                (m_data[i - n_block    ] << L_shift) |
                                                (m_data[i - n_block - 1] >> R_shift)
                                        ;
                                }
                                m_data[n_block] = m_data[0] << L_shift;
                        }
                        std::fill_n(m_data, n_block, detail::zero<word_type>);
                }
                sanitize_back();
                return *this;
        }

        PP_STL_CONSTEXPR_INCOMPLETE auto& operator>>=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_words == 1) {
                        m_data >>= n;
                } else if constexpr (num_words >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / word_size;
                        auto const R_shift = n % word_size;

                        if (R_shift == 0) {
                                std::copy_n(m_data + n_block, num_words - n_block, m_data);
                        } else {
                                auto const L_shift = word_size - R_shift;

                                for (auto i = 0; i < num_words - 1 - n_block; ++i) {
                                        m_data[i] =
                                                (m_data[i + n_block    ] >> R_shift) |
                                                (m_data[i + n_block + 1] << L_shift)
                                        ;
                                }
                                m_data[num_words - 1 - n_block] = m_data[num_words - 1] >> R_shift;
                        }
                        using std::rbegin;
                        std::fill_n(rbegin(m_data), n_block, detail::zero<word_type>);
                }
                return *this;
        }

        template<class HashAlgorithm>
        friend auto hash_append(HashAlgorithm& h, int_set const& is)
        {
                h(is.data(), is.capacity() /  std::numeric_limits<unsigned char>::digits);
        }

private:
        constexpr static auto sane = detail::ones<word_type> >> excess_bits;

        constexpr auto sanitize_back() noexcept
        {
                if constexpr (excess_bits != 0) {
                        if constexpr (num_words == 1) {
                                m_data &= sane;
                        } else {
                                static_assert(num_words >= 2);
                                m_data[num_words - 1] &= sane;
                        }
                }
        }

        constexpr auto data() const noexcept
        {
                if constexpr (num_words == 1) {
                        return &m_data;
                } else {
                        return m_data;
                }
        }

        constexpr static auto which(value_type const n) // Throws: Nothing.
        {
                static_assert(num_words != 1);
                assert(0 <= n); assert(n < num_bits);
                return n / word_size;
        }

        constexpr static auto where(value_type const n) // Throws: Nothing.
        {
                static_assert(num_words != 1);
                assert(0 <= n); assert(n < num_bits);
                return n % word_size;
        }

        friend PP_STL_CONSTEXPR_INCOMPLETE auto operator==   <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend PP_STL_CONSTEXPR_INCOMPLETE auto operator<    <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend PP_STL_CONSTEXPR_INCOMPLETE auto intersects   <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend PP_STL_CONSTEXPR_INCOMPLETE auto is_subset_of <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
};

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator==(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return true;
        } else if constexpr (num_words == 1) {
                return lhs.m_data == rhs.m_data;
        } else if constexpr (num_words >= 2) {
                return std::equal(
                        lhs.m_data, lhs.m_data + num_words,
                        rhs.m_data, rhs.m_data + num_words
                );
        }
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator!=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator<(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return false;
        } else if constexpr (num_words == 1) {
                return lhs.m_data < rhs.m_data;
        } else if constexpr (num_words >= 2) {
                using std::crbegin; using std::crend;
                return std::lexicographical_compare(
                        crbegin(lhs.m_data), crend(lhs.m_data),
                        crbegin(rhs.m_data), crend(rhs.m_data)
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
        return !(lhs < rhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto operator<=(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto swap(int_set<N>& lhs, int_set<N>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
        lhs.swap(rhs);
}

template<int N>
constexpr auto operator~(int_set<N> const& lhs) noexcept
{
        auto nrv{lhs}; nrv.toggle(); return nrv;
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
PP_STL_CONSTEXPR_INCOMPLETE auto intersects(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr static auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return false;
        } else if constexpr (num_words == 1) {
                return lhs.m_data & rhs.m_data;
        } else if constexpr (num_words >= 2) {
                return !std::equal(
                        lhs.m_data, lhs.m_data + num_words,
                        rhs.m_data, rhs.m_data + num_words,
                        [](auto const wL, auto const wR) {
                                return !(wL & wR);
                        }
                );
        }
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto disjoint(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto is_subset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        constexpr static auto num_words = int_set<N>::num_words;
        if constexpr (num_words == 0) {
                return true;
        } else if constexpr (num_words == 1) {
                return !(lhs.m_data & ~rhs.m_data);
        } else if constexpr (num_words >= 2) {
                return std::equal(
                        lhs.m_data, lhs.m_data + num_words,
                        rhs.m_data, rhs.m_data + num_words,
                        [](auto const wL, auto const wR) {
                                return !(wL & ~wR);
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
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<int N>
PP_STL_CONSTEXPR_INCOMPLETE auto is_proper_superset_of(int_set<N> const& lhs, int_set<N> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
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

template<int N, class UnaryFunction>
constexpr auto for_each(int_set<N> const& is, UnaryFunction fun)
{
        return is.for_each(fun);
}

template<int N, class UnaryFunction>
constexpr auto reverse_for_each(int_set<N> const& is, UnaryFunction fun)
{
        return is.reverse_for_each(fun);
}

template<int N>
constexpr auto count(int_set<N> const& is)
        -> decltype(is.count())
{
        return is.count();
}

template<int N>
constexpr auto empty(int_set<N> const& is)
        -> decltype(is.empty())
{
        return is.empty();
}

}       // namespace xstd

#undef PP_STL_CONSTEXPR_INCOMPLETE
