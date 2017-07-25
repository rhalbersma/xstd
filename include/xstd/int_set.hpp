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
#include <istream>              // basic_istream
#include <iterator>             // bidirectional_iterator_tag, crbegin, crend, rbegin, reverse_iterator
#include <limits>               // digits
#include <locale>               // ctype, use_facet
#include <memory>               // allocator_traits
#include <numeric>              // accumulate
#include <ostream>              // basic_ostream
#include <stdexcept>            // out_of_range
#include <string>               // basic_string, char_traits
#include <type_traits>          // conditional_t, is_integral_v, is_nothrow_swappable_v, is_pod_v, is_unsigned_v
#include <utility>              // move, swap

#if defined(__GNUG__)

        #define PP_CONSTEXPR_INLINE             constexpr
        #define PP_CONSTEXPR_CONST              constexpr
        #define PP_CONSTEXPR_CONST_INLINE       constexpr

#elif defined(_MSC_VER)

        #define PP_CONSTEXPR_INLINE             inline
        #define PP_CONSTEXPR_CONST              const
        #define PP_CONSTEXPR_CONST_INLINE       const inline

#endif

namespace xstd {
namespace detail {

#if defined(__GNUG__)

        template<int N>
        constexpr auto get(__uint128_t x) noexcept
        {
                static_assert(0 <= N); static_assert(N < 2);
                return static_cast<uint64_t>(x >> (64 * N));
        }

#elif defined(_MSC_VER)

        #include <intrin.h>

        #pragma intrinsic(_BitScanForward)
        #pragma intrinsic(_BitScanReverse)
        #pragma intrinsic(__popcnt)

        #if defined(_WIN64)

                #pragma intrinsic(_BitScanForward64)
                #pragma intrinsic(_BitScanReverse64)
                #pragma intrinsic(__popcnt64)

        #endif

#endif

template<class UIntType> constexpr auto zero =  static_cast<UIntType>(0);
template<class UIntType> constexpr auto ones = ~static_cast<UIntType>(0);

namespace builtin {

#if defined(__GNUG__)

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

        using bsfnz = ctznz;

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

#elif defined(_MSC_VER)

        struct bsfnz
        {
                auto operator()(unsigned long x) const noexcept
                {
                        assert(x != 0);
                        unsigned long index;
                        _BitScanForward(&index, x);
                        return static_cast<int>(index);
                }

        #if defined(_WIN64)

                auto operator()(uint64_t x) const noexcept
                {
                        assert(x != 0);
                        unsigned long index;
                        _BitScanForward64(&index, x);
                        return static_cast<int>(index);
                }

        #endif

        };

        using ctznz = bsfnz;

        struct bsrnz
        {
                auto operator()(unsigned long x) const noexcept
                {
                        assert(x != 0);
                        unsigned long index;
                        _BitScanReverse(&index, x);
                        return static_cast<int>(index);
                }

        #if defined(_WIN64)

                auto operator()(uint64_t x) const noexcept
                {
                        assert(x != 0);
                        unsigned long index;
                        _BitScanReverse64(&index, x);
                        return static_cast<int>(index);
                }

        #endif

        };

        struct popcount
        {
                auto operator()(unsigned x) const noexcept
                {
                        return __popcnt(x);
                }

        #if defined(_WIN64)

                auto operator()(uint64_t x) const noexcept
                {
                        return __popcnt64(x);
                }

        #endif

        };

#endif

}       // namespace builtin

template<class UIntType>
PP_CONSTEXPR_INLINE auto ctznz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::ctznz{}(x);
}

template<class UIntType>
PP_CONSTEXPR_INLINE auto bsfnz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::bsfnz{}(x);
}

template<class UIntType>
PP_CONSTEXPR_INLINE auto ctz(UIntType x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<UIntType>::digits;
}

template<class UIntType>
PP_CONSTEXPR_INLINE auto bsf(UIntType x) noexcept
{
        return x ? bsfnz(x) : std::numeric_limits<UIntType>::digits;
}

#if defined(__GNUG__)

        template<class UIntType>
        PP_CONSTEXPR_INLINE auto clznz(UIntType x) // Throws: Nothing.
        {
                assert(x != 0);
                return builtin::clznz{}(x);
        }

        template<class UIntType>
        PP_CONSTEXPR_INLINE auto bsrnz(UIntType x) // Throws: Nothing.
        {
                assert(x != 0);
                return std::numeric_limits<UIntType>::digits - 1 - builtin::clznz{}(x);
        }

#elif defined(_MSC_VER)

        template<class UIntType>
        PP_CONSTEXPR_INLINE auto clznz(UIntType x) // Throws: Nothing.
        {
                assert(x != 0);
                return std::numeric_limits<UIntType>::digits - 1 - builtin::bsrnz{}(x);
        }

        template<class UIntType>
        PP_CONSTEXPR_INLINE auto bsrnz(UIntType x) // Throws: Nothing.
        {
                assert(x != 0);
                return builtin::bsrnz{}(x);
        }

#endif

template<class UIntType>
PP_CONSTEXPR_INLINE auto clz(UIntType x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<UIntType>::digits;
}

template<class UIntType>
PP_CONSTEXPR_INLINE auto bsr(UIntType x) noexcept
{
        return x ? bsrnz(x) : -1;
}

template<class UIntType>
PP_CONSTEXPR_INLINE auto popcount(UIntType x) noexcept
{
        return builtin::popcount{}(x);
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

template<int, class>
class int_set;

template<int N, class UIntType> auto operator==  (int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;
template<int N, class UIntType> auto operator<   (int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;
template<int N, class UIntType> auto is_subset_of(int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;
template<int N, class UIntType> auto intersects  (int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;

template<int N, class UIntType = unsigned long long>
class int_set
{
public:
        using block_type = UIntType;
private:
        static_assert(0 <= N);
        static_assert(std::is_unsigned_v<block_type>);
        static_assert(std::is_integral_v<block_type>);
        static_assert(sizeof(unsigned) <= sizeof(UIntType));

        constexpr static auto block_size = std::numeric_limits<block_type>::digits;
        constexpr static auto num_blocks = (N - 1 + block_size) / block_size;
        constexpr static auto num_bits = num_blocks * block_size;
        constexpr static auto excess_bits = num_bits - N;

        using data_type = std::conditional_t<num_blocks == 1, block_type, block_type[std::max(num_blocks, 1)]>;
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

                block_type const& m_block;
                value_type const m_value;

        public:
                ~const_reference() = default;
                const_reference(const_reference const&) = default;
                const_reference(const_reference&&) = default;
                const_reference& operator=(const_reference const&) = delete;
                const_reference& operator=(const_reference&&) = delete;

                const_reference() = delete;

                constexpr const_reference(block_type const& b, value_type const v) noexcept
                :
                        m_block{b},
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
                        return { &m_block, m_value };
                }
        };

        class const_iterator
        {
        public:
                using difference_type   = typename int_set::difference_type;
                using value_type        = typename int_set::value_type;
                using pointer           = const_iterator;
                using reference         = const_reference;
                using iterator_category = std::bidirectional_iterator_tag;

        private:
                constexpr auto assert_invariants() const noexcept
                {
                        assert(m_block != nullptr);
                        assert(0 <= m_value); assert(m_value < N || m_value == num_bits);
                }

                block_type const* m_block;
                value_type m_value;

        public:
                const_iterator() = default;

                constexpr const_iterator(block_type const* b, value_type const v) // Throws: Nothing.
                :
                        m_block{b},
                        m_value{v}
                {
                        assert_invariants();
                }

                constexpr auto operator*() const // Throws: Nothing.
                        -> const_reference
                {
                        assert(m_value < N);
                        return { *m_block, m_value };
                }

                PP_CONSTEXPR_INLINE auto& operator++() // Throws: Nothing.
                {
                        increment();
                        assert_invariants();
                        return *this;
                }

                PP_CONSTEXPR_INLINE auto operator++(int) // Throws: Nothing.
                {
                        auto nrv = *this; ++*this; return nrv;
                }

                PP_CONSTEXPR_INLINE auto& operator--() // Throws:Nothing.
                {
                        decrement();
                        assert_invariants();
                        return *this;
                }

                PP_CONSTEXPR_INLINE auto operator--(int) // Throws: Nothing.
                {
                        auto nrv = *this; --*this; return nrv;
                }

                friend constexpr auto operator==(const_iterator const& lhs, const_iterator const& rhs) noexcept
                {
                        assert(lhs.m_block == rhs.m_block);
                        return lhs.m_value == rhs.m_value;
                }

                friend constexpr auto operator!=(const_iterator const& lhs, const_iterator const& rhs) noexcept
                {
                        assert(lhs.m_block == rhs.m_block);
                        return !(lhs == rhs);
                }

        private:
                PP_CONSTEXPR_INLINE auto increment() // Throws: Nothing.
                {
                        assert(m_value < N);
                        if (++m_value == num_bits) { return; }
                        if constexpr (num_blocks == 1) {
                                if (auto const block = *m_block >> m_value; block != zero) {
                                        m_value += detail::ctznz(block);
                                        return;
                                }
                                m_value = block_size;
                        } else if constexpr (num_blocks >= 2) {
                                auto i = which(m_value);
                                if (auto const offset = where(m_value); offset != 0) {
                                        if (auto const block = m_block[i] >> offset; block != zero) {
                                                m_value += detail::ctznz(block);
                                                return;
                                        }
                                        ++i;
                                        m_value += block_size - offset;
                                }
                                for (/* initialized before loop */; i < num_blocks; ++i, m_value += block_size) {
                                        if (auto const block = m_block[i]; block != zero) {
                                                m_value += detail::ctznz(block);
                                                return;
                                        }
                                }
                        }
                        assert(m_value == num_bits);
                }

                PP_CONSTEXPR_INLINE auto decrement() // Throws: Nothing.
                {
                        assert(0 < m_value);
                        --m_value;
                        if constexpr (num_blocks == 1) {
                                m_value -= detail::clznz(*m_block << (block_size - 1 - m_value));
                        } else if constexpr (num_blocks >= 2) {
                                auto i = which(m_value);
                                if (auto const offset = where(m_value); offset != block_size - 1) {
                                        if (auto const block = m_block[i] << (block_size - 1 - offset); block != zero) {
                                                m_value -= detail::clznz(block);
                                                return;
                                        }
                                        --i;
                                        m_value -= offset + 1;
                                }
                                for (/* initialized before loop */; i >= 0; --i, m_value -= block_size) {
                                        if (auto const block = m_block[i]; block != zero) {
                                                m_value -= detail::clznz(block);
                                                return;
                                        }
                                }
                        }
                        assert(m_value == 0);
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

        [[deprecated]] /* implicit */ constexpr int_set(unsigned long long const /* val */) noexcept
        :
                m_data{}
        {

        }

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

        template<class InputIterator>
        auto assign(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                clear();
                insert(first, last);
        }

        auto& operator=(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                assign(ilist.begin(), ilist.end());
                return *this;
        }

        auto to_ulong() const
        {

        }

        auto to_ullong() const
        {

        }

        template<class charT = char, class traits = std::char_traits<charT>, class Allocator = std::allocator<charT>>
        auto to_string(charT nil = charT('0'), charT one = charT('1')) const
        {
                auto str = std::basic_string<charT, traits, Allocator>(N, nil);
                for (auto i = 0; i < N; ++i) {
                        if (contains(i)) {
                                traits::assign(str[static_cast<std::size_t>(N - 1 - i)], one);
                        }
                }
                return str;
        }

        constexpr auto begin()         noexcept { return       iterator{data(), find_first()}; }
        constexpr auto begin()   const noexcept { return const_iterator{data(), find_first()}; }
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

        PP_CONSTEXPR_INLINE auto front() const // Throws: Nothing.
                -> const_reference
        {
                assert(!empty());
                return { *data(), find_first() };
        }

        PP_CONSTEXPR_INLINE auto back() const // Throws: Nothing.
                -> const_reference
        {
                assert(!empty());
                return { *data(), find_last() };
        }

        template<class UnaryFunction>
        PP_CONSTEXPR_INLINE auto for_each(UnaryFunction fun) const
        {
                if constexpr (num_blocks == 1) {
                        for (auto block = m_data; block != zero; /* update inside loop */) {
                                auto const first = detail::bsfnz(block);
                                fun(first);
                                block ^= bit1(first);
                        }
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0, offset = 0; i < num_blocks; ++i, offset += block_size) {
                                for (auto block = m_data[i]; block != zero; /* update inside loop */) {
                                        auto const first = detail::bsfnz(block);
                                        fun(offset + first);
                                        block ^= bit1(first);
                                }
                        }
                }
                return std::move(fun);
        }

        template<class UnaryFunction>
        PP_CONSTEXPR_INLINE auto reverse_for_each(UnaryFunction fun) const
        {
                if constexpr (num_blocks == 1) {
                        for (auto block = m_data; block != zero; /* update inside loop */) {
                                auto const last = detail::bsrnz(block);
                                fun(last);
                                block ^= bit1(last);
                        }
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = num_blocks - 1, offset = (num_blocks - 1) * block_size; i >= 0; --i, offset -= block_size) {
                                for (auto block = m_data[i]; block != zero; /* update inside loop */) {
                                        auto const last = detail::bsrnz(block);
                                        fun(offset + last);
                                        block ^= bit1(last);
                                }
                        }
                }
                return std::move(fun);
        }

        auto full() const noexcept
        {
                if constexpr (excess_bits == 0) {
                        if constexpr (num_blocks == 0) {
                                return true;
                        } else if constexpr (num_blocks == 1) {
                                return m_data == ones;
                        } else if constexpr (num_blocks >= 2) {
                                return std::all_of(m_data, m_data + num_blocks, [](auto const block) {
                                        return block == ones;
                                });
                        }
                } else {
                        if constexpr (num_blocks == 1) {
                                return m_data == sane;
                        } else {
                                static_assert(num_blocks >= 2);
                                return
                                        std::all_of(m_data, m_data + num_blocks - 1, [](auto const block) {
                                                return block == ones;
                                        }) && m_data[num_blocks - 1] == sane;
                                ;
                        }
                }
        }

        auto empty() const noexcept
        {
                if constexpr (num_blocks == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return m_data == zero;
                } else if constexpr (num_blocks >= 2) {
                        return std::all_of(m_data, m_data + num_blocks, [](auto const block) {
                                return block == zero;
                        });
                }
        }

        [[deprecated]] auto all() const noexcept { return full(); }
        [[deprecated]] auto any() const noexcept { return !empty(); }
        [[deprecated]] auto none() const noexcept { return empty(); }

        auto count() const noexcept
        {
                if constexpr (num_blocks == 0) {
                        return 0;
                } else if constexpr (num_blocks == 1) {
                        return detail::popcount(m_data);
                } else if (num_blocks >= 2) {
                        return std::accumulate(m_data, m_data + num_blocks, 0, [](auto const sum, auto const block) {
                                return sum + detail::popcount(block);
                        });
                }
        }

        constexpr static auto max_size() noexcept { return N; }
        constexpr static auto capacity() noexcept { return num_bits; }

        [[deprecated]] constexpr auto size() const noexcept { return max_size(); }

        constexpr auto& insert(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data |= bit1(n);
                } else {
                        m_data[which(n)] |= bit1(where(n));
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

        auto fill() noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data = ones;
                } else if constexpr (num_blocks >= 2) {
                        std::fill_n(m_data, num_blocks, ones);
                }
                sanitize_back();
                assert(full());
        }

        constexpr auto& erase(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data &= ~bit1(n);
                } else {
                        m_data[which(n)] &= ~bit1(where(n));
                }
                assert(!contains(n));
                return *this;
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

        auto swap(int_set& other) noexcept(num_blocks == 0 || std::is_nothrow_swappable_v<value_type>)
        {
                if constexpr (num_blocks == 1) {
                        using std::swap;
                        swap(m_data, other.m_data);
                } else if constexpr (num_blocks >= 2) {
                        std::swap_ranges(m_data, m_data + num_blocks, other.m_data);
                }
        }

        auto clear() noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data = zero;
                } else if constexpr (num_blocks >= 2) {
                        std::fill_n(m_data, num_blocks, zero);
                }
                assert(empty());
        }

        constexpr auto& toggle(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data ^= bit1(n);
                } else {
                        m_data[which(n)] ^= bit1(where(n));
                }
                return *this;
        }

        constexpr auto& toggle() noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data = ~m_data;
                } else if constexpr (num_blocks >= 2) {
                        for (auto&& block : m_data) {
                                block = ~block;
                        }
                }
                sanitize_back();
                return *this;
        }

        constexpr auto contains(value_type const n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        return (m_data & bit1(n)) != zero;
                } else {
                        return (m_data[which(n)] & bit1(where(n))) != zero;
                }
        }

        [[deprecated]] auto& set(size_type const pos, bool const val = true)
        {
                if (static_cast<std::size_t>(pos) >= static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N, UIntType>::set(): index out of range"};
                }
                return val ? insert(pos) : erase(pos);
        }

        [[deprecated]] auto& set() noexcept
        {
                fill();
                return *this;
        }

        [[deprecated]] auto& reset(size_type const pos)
        {
                if (static_cast<std::size_t>(pos) >= static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N, UIntType>::reset(): index out of range"};
                }
                return erase(pos);
        }

        [[deprecated]] auto& reset() noexcept
        {
                clear();
                return *this;
        }

        [[deprecated]] auto& flip(size_type const pos)
        {
                if (static_cast<std::size_t>(pos) >= static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N, UIntType>::flip(): index out of range"};
                }
                return toggle(pos);
        }

        [[deprecated]] auto& flip() noexcept
        {
                return toggle();
        }

        [[deprecated]] constexpr auto operator[](size_type const pos) const // Throws: Nothing.
        {
                return contains(pos);
        }

        [[deprecated]] auto test(size_type const pos) const
        {
                if (static_cast<std::size_t>(pos) >= static_cast<std::size_t>(N)) {
                        throw std::out_of_range{"int_set<N, UIntType>::test(): index out of range"};
                }
                return contains(pos);
        }

        constexpr auto& operator&=(int_set const& other) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data &= other.m_data;
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] &= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(int_set const& other) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data |= other.m_data;
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] |= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(int_set const& other) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data ^= other.m_data;
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] ^= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(int_set const& other) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data &= ~other.m_data;
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] &= ~other.m_data[i];
                        }
                }
                return *this;
        }

        auto& operator<<=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data <<= n;
                } else if constexpr (num_blocks >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / block_size;
                        auto const L_shift = n % block_size;

                        if (L_shift == 0) {
                                std::copy_backward(m_data, m_data + num_blocks - n_block, m_data + num_blocks);
                        } else {
                                auto const R_shift = block_size - L_shift;

                                for (auto i = num_blocks - 1; i > n_block; --i) {
                                        m_data[i] =
                                                (m_data[i - n_block    ] << L_shift) |
                                                (m_data[i - n_block - 1] >> R_shift)
                                        ;
                                }
                                m_data[n_block] = m_data[0] << L_shift;
                        }
                        std::fill_n(m_data, n_block, zero);
                }
                sanitize_back();
                return *this;
        }

        auto& operator>>=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data >>= n;
                } else if constexpr (num_blocks >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / block_size;
                        auto const R_shift = n % block_size;

                        if (R_shift == 0) {
                                std::copy_n(m_data + n_block, num_blocks - n_block, m_data);
                        } else {
                                auto const L_shift = block_size - R_shift;

                                for (auto i = 0; i < num_blocks - 1 - n_block; ++i) {
                                        m_data[i] =
                                                (m_data[i + n_block    ] >> R_shift) |
                                                (m_data[i + n_block + 1] << L_shift)
                                        ;
                                }
                                m_data[num_blocks - 1 - n_block] = m_data[num_blocks - 1] >> R_shift;
                        }
                        using std::rbegin;
                        std::fill_n(rbegin(m_data), n_block, zero);
                }
                return *this;
        }

        template<class HashAlgorithm>
        friend auto hash_append(HashAlgorithm& h, int_set const& is)
        {
                h(is.data(), is.capacity() /  std::numeric_limits<unsigned char>::digits);
        }

private:
        constexpr static auto zero = detail::zero<block_type>;
        constexpr static auto ones = detail::ones<block_type>;
        constexpr static auto sane = ones >> excess_bits;

        constexpr auto sanitize_back() noexcept
        {
                if constexpr (excess_bits != 0) {
                        if constexpr (num_blocks == 1) {
                                m_data &= sane;
                        } else {
                                static_assert(num_blocks >= 2);
                                m_data[num_blocks - 1] &= sane;
                        }
                }
        }

        constexpr auto data() const noexcept
        {
                if constexpr (num_blocks == 1) {
                        return &m_data;
                } else {
                        return m_data;
                }
        }

        PP_CONSTEXPR_INLINE auto find_first() const noexcept
        {
                if constexpr (num_blocks == 0) {
                        return 0;
                } else if constexpr (num_blocks == 1) {
                        return detail::bsf(m_data);
                } else if constexpr (num_blocks >= 2) {
                        auto offset = 0;
                        for (auto i = 0; i < num_blocks; ++i, offset += block_size) {
                                if (auto const block = m_data[i]; block != zero) {
                                        offset += detail::ctznz(block);
                                        break;
                                }
                        }
                        return offset;
                }
        }

        PP_CONSTEXPR_INLINE auto find_last() const noexcept
        {
                if constexpr (num_blocks == 1) {
                        return detail::bsr(m_data);
                } else {
                        auto offset = num_bits - 1;
                        for (auto i = num_blocks - 1; i > -1; --i, offset -= block_size) {
                                if (auto const block = m_data[i]; block != zero) {
                                        offset -= detail::clznz(block);
                                        break;
                                }
                        }
                        return offset;
                }
        }

        constexpr static auto bit1(value_type const n)  // Throws: Nothing.
        {
                return detail::bit1<block_type>(n);
        }

        constexpr static auto which(value_type const n) // Throws: Nothing.
        {
                static_assert(num_blocks != 1);
                assert(0 <= n); assert(n < num_bits);
                return n / block_size;
        }

        constexpr static auto where(value_type const n) // Throws: Nothing.
        {
                static_assert(num_blocks != 1);
                assert(0 <= n); assert(n < num_bits);
                return n % block_size;
        }

        friend auto operator==  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend auto operator<   <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend auto is_subset_of<>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend auto intersects  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
};

template<int N, class UIntType>
auto operator==(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        constexpr auto num_blocks = int_set<N, UIntType>::num_blocks;
        if constexpr (num_blocks == 0) {
                return true;
        } else if constexpr (num_blocks == 1) {
                return lhs.m_data == rhs.m_data;
        } else if constexpr (num_blocks >= 2) {
                return std::equal(
                        lhs.m_data, lhs.m_data + num_blocks,
                        rhs.m_data, rhs.m_data + num_blocks
                );
        }
}

template<int N, class UIntType>
auto operator!=(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N, class UIntType>
auto operator<(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        constexpr auto num_blocks = int_set<N, UIntType>::num_blocks;
        if constexpr (num_blocks == 0) {
                return false;
        } else if constexpr (num_blocks == 1) {
                return lhs.m_data < rhs.m_data;
        } else if constexpr (num_blocks >= 2) {
                using std::crbegin; using std::crend;
                return std::lexicographical_compare(
                        crbegin(lhs.m_data), crend(lhs.m_data),
                        crbegin(rhs.m_data), crend(rhs.m_data)
                );
        }
}

template<int N, class UIntType>
auto operator>(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return rhs < lhs;
}

template<int N, class UIntType>
auto operator>=(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<int N, class UIntType>
auto operator<=(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template<int N, class UIntType>
auto is_subset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        constexpr static auto num_blocks = int_set<N, UIntType>::num_blocks;
        constexpr static auto zero [[maybe_unused]] = detail::zero<typename int_set<N, UIntType>::block_type>;
        if constexpr (num_blocks == 0) {
                return true;
        } else if constexpr (num_blocks == 1) {
                return (lhs.m_data & ~rhs.m_data) == zero;
        } else if constexpr (num_blocks >= 2) {
                return std::equal(
                        lhs.m_data, lhs.m_data + num_blocks,
                        rhs.m_data, rhs.m_data + num_blocks,
                        [](auto const wL, auto const wR) {
                                return (wL & ~wR) == zero;
                        }
                );
        }
}

template<int N, class UIntType>
auto is_superset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<int N, class UIntType>
auto is_proper_subset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<int N, class UIntType>
auto is_proper_superset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
}

template<int N, class UIntType>
auto intersects(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        constexpr static auto num_blocks = int_set<N, UIntType>::num_blocks;
        constexpr static auto zero [[maybe_unused]] = detail::zero<typename int_set<N, UIntType>::block_type>;
        if constexpr (num_blocks == 0) {
                return false;
        } else if constexpr (num_blocks == 1) {
                return (lhs.m_data & rhs.m_data) != zero;
        } else if constexpr (num_blocks >= 2) {
                return !std::equal(
                        lhs.m_data, lhs.m_data + num_blocks,
                        rhs.m_data, rhs.m_data + num_blocks,
                        [](auto const wL, auto const wR) {
                                return (wL & wR) == zero;
                        }
                );
        }
}

template<int N, class UIntType>
auto disjoint(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<int N, class UIntType>
auto swap(int_set<N, UIntType>& lhs, int_set<N, UIntType>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
        lhs.swap(rhs);
}

template<int N, class UIntType>
constexpr auto operator~(int_set<N, UIntType> const& lhs) noexcept
{
        auto nrv{lhs}; nrv.toggle(); return nrv;
}

template<int N, class UIntType>
constexpr auto operator&(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv &= rhs; return nrv;
}

template<int N, class UIntType>
constexpr auto operator|(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv |= rhs; return nrv;
}

template<int N, class UIntType>
constexpr auto operator^(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv ^= rhs; return nrv;
}

template<int N, class UIntType>
constexpr auto operator-(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv -= rhs; return nrv;
}

template<int N, class UIntType>
auto operator<<(int_set<N, UIntType> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv <<= n; return nrv;
}

template<int N, class UIntType>
auto operator>>(int_set<N, UIntType> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv >>= n; return nrv;
}

template<int N, class UIntType>
constexpr auto begin(int_set<N, UIntType>& is)
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N, class UIntType>
constexpr auto begin(int_set<N, UIntType> const& is)
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N, class UIntType>
constexpr auto end(int_set<N, UIntType>& is)
        -> decltype(is.end())
{
        return is.end();
}

template<int N, class UIntType>
constexpr auto end(int_set<N, UIntType> const& is)
        -> decltype(is.end())
{
        return is.end();
}

template<int N, class UIntType>
constexpr auto cbegin(int_set<N, UIntType> const& is) noexcept(noexcept(xstd::begin(is)))
        -> decltype(xstd::begin(is))
{
        return xstd::begin(is);
}

template<int N, class UIntType>
constexpr auto cend(int_set<N, UIntType> const& is) noexcept(noexcept(xstd::end(is)))
        -> decltype(xstd::end(is))
{
        return xstd::end(is);
}

template<int N, class UIntType>
constexpr auto rbegin(int_set<N, UIntType>& is)
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N, class UIntType>
constexpr auto rbegin(int_set<N, UIntType> const& is)
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N, class UIntType>
constexpr auto rend(int_set<N, UIntType>& is)
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N, class UIntType>
constexpr auto rend(int_set<N, UIntType> const& is)
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N, class UIntType>
constexpr auto crbegin(int_set<N, UIntType> const& is)
        -> decltype(xstd::rbegin(is))
{
        return xstd::rbegin(is);
}

template<int N, class UIntType>
constexpr auto crend(int_set<N, UIntType> const& is)
        -> decltype(xstd::rend(is))
{
        return xstd::rend(is);
}

template<int N, class UIntType, class UnaryFunction>
constexpr auto for_each(int_set<N, UIntType> const& is, UnaryFunction fun)
{
        return is.for_each(fun);
}

template<int N, class UIntType, class UnaryFunction>
constexpr auto reverse_for_each(int_set<N, UIntType> const& is, UnaryFunction fun)
{
        return is.reverse_for_each(fun);
}

template<int N, class UIntType>
constexpr auto empty(int_set<N, UIntType> const& is)
        -> decltype(is.empty())
{
        return is.empty();
}

template<class charT, class traits, int N, class UIntType>
auto& operator>>(std::basic_istream<charT, traits>& istr, int_set<N, UIntType>& /* is */)
{
        return istr;
}

template<class charT, class traits, int N, class UIntType>
auto& operator<<(std::basic_ostream<charT, traits>& ostr, int_set<N, UIntType> const& is)
{
        return ostr << is.template to_string<charT, traits, std::allocator<charT>>(
                std::use_facet<std::ctype<charT>>(ostr.getloc()).widen('0'),
                std::use_facet<std::ctype<charT>>(ostr.getloc()).widen('1')
        );
}

}       // namespace xstd
