#pragma once
#include <xstd/word_array/word_array_fwd.hpp>     // word_array
#include <xstd/bit/mask.hpp>                    // none, one, all
#include <xstd/bit/primitive.hpp>               // ctznz, popcount
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <cassert>                              // assert
#include <limits>                               // digits
#include <type_traits>                          // is_nothrow_swappable
#include <utility>                              // move, swap

namespace xstd {

template<class T>
struct word_array<T, 1>
{
        T m_word;

        // types:
        using value_type             = T;
        using pointer                = T*;
        using const_pointer          = T const*;
        using reference              = T&;
        using const_reference        = T const&;
        using size_type              = int;
        using difference_type        = int;
        using iterator               = pointer;
        using const_iterator         = const_pointer;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // no explicit construct/copy/destroy for aggregate type

        constexpr auto fill(value_type const& u) noexcept
        {
                m_word = u;
        }

        auto swap(word_array& other) noexcept
        {
                using std::swap;
                swap(m_word, other.m_word);
        }

        // iterators:
        constexpr auto begin()         noexcept { return       iterator(data()); }
        constexpr auto begin()   const noexcept { return const_iterator(data()); }
        constexpr auto end()           noexcept { return       iterator(data() + 1); }
        constexpr auto end()     const noexcept { return const_iterator(data() + 1); }
        constexpr auto rbegin()        noexcept { return       reverse_iterator(end()); }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator(end()); }
        constexpr auto rend()          noexcept { return       reverse_iterator(begin()); }
        constexpr auto rend()    const noexcept { return const_reverse_iterator(begin()); }
        constexpr auto cbegin()  const noexcept { return       iterator(data());     }
        constexpr auto cend()    const noexcept { return const_iterator(data() + 1); }
        constexpr auto crbegin() const noexcept { return const_reverse_iterator(end());   }
        constexpr auto crend()   const noexcept { return const_reverse_iterator(begin()); }

        // capacity:
        constexpr auto empty()     const noexcept { return false; }
        constexpr auto size()      const noexcept { return 1;     }
        constexpr auto max_size()  const noexcept { return 1;     }
        static constexpr auto word_size() noexcept { return std::numeric_limits<value_type>::digits; }

        // element access:
        constexpr       reference operator[](size_type const n)       { assert(n == 0); return m_word; }
        constexpr const_reference operator[](size_type const n) const { assert(n == 0); return m_word; }
        constexpr       reference front()       { return m_word; }
        constexpr const_reference front() const { return m_word; }
        constexpr       reference back()       { return m_word; }
        constexpr const_reference back() const { return m_word; }
        constexpr auto data()       noexcept { return &m_word; }
        constexpr auto data() const noexcept { return &m_word; }

        // modifiers

        constexpr auto& flip() noexcept
        {
                m_word = ~m_word;
                return *this;
        }

        constexpr auto& operator&=(word_array const& other) noexcept
        {
                m_word &= other.m_word;
                return *this;
        }

        constexpr auto operator|=(word_array const& other) noexcept
        {
                m_word |= other.m_word;
                return *this;
        }

        constexpr auto operator^=(word_array const& other) noexcept
        {
                m_word ^= other.m_word;
                return *this;
        }

        constexpr auto operator-=(word_array const& other) noexcept
        {
                m_word &= ~other.m_word;
                return *this;
        }

        constexpr auto operator<<=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < size() * word_size());
                m_word <<= n;
                return *this;
        }

        constexpr auto operator>>=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < size() * word_size());
                m_word >>= n;
                return *this;
        }

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction f) const
        {
                for (auto word = m_word; word;) {
                        auto const first = bit::bsfnz(word);
                        f(first);
                        word ^= bit::mask::one<value_type> << first;
                }
                return std::move(f);
        }

        template<class UnaryFunction>
        constexpr auto reverse_for_each(UnaryFunction f) const
        {
                for (auto word = m_word; word;) {
                        auto const last = bit::bsrnz(word);
                        f(last);
                        word ^= bit::mask::one<value_type> << last;
                }
                return std::move(f);
        }

        // observers

        template<int M = 0>
        constexpr auto all() const noexcept
        {
                static_assert(0 <= M); static_assert(M < word_size());
                if constexpr (M == 0) {
                        return m_word == bit::mask::all<value_type>;
                } else {
                        return m_word == bit::mask::all<value_type> >> (word_size() - M);
                }
        }

        constexpr auto any() const noexcept
        {
                return m_word != bit::mask::none<value_type>;
        }

        constexpr auto none() const noexcept
        {
                return m_word == bit::mask::none<value_type>;
        }

        constexpr auto count() const noexcept
        {
                return bit::popcount(m_word);
        }
};

template<class T>
constexpr auto operator==(word_array<T, 1> const& lhs, word_array<T, 1> const& rhs) noexcept
{
        return lhs.m_word == rhs.m_word;
}

template<class T>
constexpr auto operator<(word_array<T, 1> const& lhs, word_array<T, 1> const& rhs) noexcept
{
        return lhs.m_word < rhs.m_word;
}

template<class T>
constexpr auto intersects(word_array<T, 1> const& lhs, word_array<T, 1> const& rhs) noexcept
        -> bool
{
        return lhs.m_word & rhs.m_word;
}

template<class T>
constexpr auto subset_of(word_array<T, 1> const& lhs, word_array<T, 1> const& rhs) noexcept
        -> bool
{
        return !(lhs.m_word & ~rhs.m_word);
}

}       // namespace xstd
