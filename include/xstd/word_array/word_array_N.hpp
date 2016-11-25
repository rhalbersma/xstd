#pragma once
#include <xstd/word_array/word_array_fwd.hpp>     // word_array
#include <xstd/bit/mask.hpp>                    // none, one, all
#include <xstd/bit/primitive.hpp>               // ctznz, popcount
#include <xstd/limits.hpp>                      // digits, is_unsigned_integer
#include <boost/algorithm/cxx11/all_of.hpp>     // all_of
#include <boost/algorithm/cxx11/any_of.hpp>     // any_of
#include <boost/algorithm/cxx11/none_of.hpp>    // none_of
#include <boost/range/adaptors.hpp>             // reverse
#include <boost/range/algorithm.hpp>            // equal, fill_n, lexicographical_compare
#include <boost/range/algorithm/swap_ranges.hpp>// swap_ranges
#include <boost/range/combine.hpp>              // combine
#include <boost/range/numeric.hpp>              // accumulate
#include <boost/tuple/tuple.hpp>                // make_tuple
#include <algorithm>                            // all_of, copy_n, copy_backward
#include <cassert>                              // assert
#include <iterator>                             // begin, end, cbegin, cend, crbegin, crend
#include <limits>                               // digits
#include <type_traits>                          // is_nothrow_swappable
#include <utility>                              // move

namespace xstd {

template<class T, int N>
struct word_array
{
        T m_words[N];

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

        auto fill(value_type const& u) noexcept
        {
                boost::fill_n(m_words, N, u);
        }

        auto swap(word_array& other) noexcept
        {
                boost::range::swap_ranges(m_words, other.m_words);
        }

        // iterators:
        constexpr auto begin()         noexcept { using std::begin;   return begin(m_words);   }
        constexpr auto begin()   const noexcept { using std::begin;   return begin(m_words);   }
        constexpr auto end()           noexcept { using std::end;     return end(m_words);     }
        constexpr auto end()     const noexcept { using std::end;     return end(m_words);     }
        constexpr auto rbegin()        noexcept { using std::rbegin;  return rbegin(m_words);  }
        constexpr auto rbegin()  const noexcept { using std::rbegin;  return rbegin(m_words);  }
        constexpr auto rend()          noexcept { using std::rend;    return rend(m_words);    }
        constexpr auto rend()    const noexcept { using std::rend;    return rend(m_words);    }
        constexpr auto cbegin()  const noexcept { using std::cbegin;  return cbegin(m_words);  }
        constexpr auto cend()    const noexcept { using std::cend;    return cend(m_words);    }
        constexpr auto crbegin() const noexcept { using std::crbegin; return crbegin(m_words); }
        constexpr auto crend()   const noexcept { using std::crend;   return crend(m_words);   }

        // capacity:
        constexpr auto empty()     const noexcept { return size() == 0; }
        constexpr auto size()      const noexcept { return N; }
        constexpr auto max_size()  const noexcept { return N; }
        static constexpr auto word_size() noexcept { return std::numeric_limits<value_type>::digits; }

        // element access:
        constexpr       reference operator[](size_type n)       { assert(0 <= n); assert(n < size()); return m_words[n]; }
        constexpr const_reference operator[](size_type n) const { assert(0 <= n); assert(n < size()); return m_words[n]; }
        constexpr       reference front()       { return m_words[0]; }
        constexpr const_reference front() const { return m_words[0]; }
        constexpr       reference back()       { return m_words[N - 1]; }
        constexpr const_reference back() const { return m_words[N - 1]; }
        constexpr auto data()       noexcept { return &m_words[0]; }
        constexpr auto data() const noexcept { return &m_words[0]; }

        // modifiers

        constexpr auto& flip() noexcept
        {
                for (auto& word : m_words)
                        word = ~word;
                return *this;
        }

        constexpr auto& operator&=(word_array const& other) noexcept
        {
                for (auto i = 0; i < N; ++i)
                        m_words[i] &= other.m_words[i];
                return *this;
        }

        constexpr auto& operator|=(word_array const& other) noexcept
        {
                for (auto i = 0; i < N; ++i)
                        m_words[i] |= other.m_words[i];
                return *this;
        }

        constexpr auto& operator^=(word_array const& other) noexcept
        {
                for (auto i = 0; i < N; ++i)
                        m_words[i] ^= other.m_words[i];
                return *this;
        }

        constexpr auto& operator-=(word_array const& other) noexcept
        {
                for (auto i = 0; i < N; ++i)
                        m_words[i] &= ~other.m_words[i];
                return *this;
        }

        auto& operator<<=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < size() * word_size());
                using std::begin; using std::end;
                if (n == 0) return *this;

                auto const n_block = n / word_size();
                auto const L_shift = n % word_size();

                if (L_shift == 0) {
                        std::copy_backward(begin(m_words), end(m_words) - n_block, end(m_words));
                } else {
                        auto const R_shift = word_size() - L_shift;

                        for (auto i = N - 1; i > n_block; --i) {
                                m_words[i] =
                                        (m_words[i - n_block    ] << L_shift) |
                                        (m_words[i - n_block - 1] >> R_shift)
                                ;
                        }
                        m_words[n_block] = m_words[0] << L_shift;
                }
                boost::fill_n(m_words, n_block, bit::mask::none<value_type>);
                return *this;
        }

        auto& operator>>=(int const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < size() * word_size());
                using std::begin; using std::end;
                using boost::adaptors::reverse;
                if (n == 0) return *this;

                auto const n_block = n / word_size();
                auto const R_shift = n % word_size();

                if (R_shift == 0) {
                        std::copy_n(begin(m_words) + n_block, N - n_block, begin(m_words));
                } else {
                        auto const L_shift = word_size() - R_shift;

                        for (auto i = 0; i < N - 1 - n_block; ++i) {
                                m_words[i] =
                                        (m_words[i + n_block    ] >> R_shift) |
                                        (m_words[i + n_block + 1] << L_shift)
                                ;
                        }
                        m_words[N - 1 - n_block] = m_words[N - 1] >> R_shift;
                }
                boost::fill_n(reverse(m_words), n_block, bit::mask::none<value_type>);
                return *this;
        }

        template<class UnaryFunction>
        constexpr auto for_each(UnaryFunction f) const
        {
                for (auto i = 0, offset = 0; i < N; ++i, offset += word_size()) {
                        for (auto word = m_words[i]; word;) {
                                auto const first = bit::bsfnz(word);
                                f(offset + first);
                                word ^= bit::mask::one<value_type> << first;
                        }
                }
                return std::move(f);
        }

        template<class UnaryFunction>
        constexpr auto reverse_for_each(UnaryFunction f) const
        {
                for (auto i = N - 1, offset = (size() - 1) * word_size(); i >= 0; --i, offset -= word_size()) {
                        for (auto word = m_words[i]; word;) {
                                auto const last = bit::bsrnz(word);
                                f(offset + last);
                                word ^= bit::mask::one<value_type> << last;
                        }
                }
                return std::move(f);
        }

        // observers

        template<int M = 0>
        auto all() const noexcept
        {
                static_assert(0 <= M); static_assert(M < word_size());
                if constexpr (M == 0) {
                        return boost::algorithm::all_of(m_words, [](auto const word){
                                return word == bit::mask::all<value_type>;
                        });
                } else {
                        using std::cbegin; using std::cend;
                        return std::all_of(cbegin(m_words), cend(m_words) - 1, [](auto const word){
                                return word == bit::mask::all<value_type>;
                        }) && (back() == bit::mask::all<value_type> >> (word_size() - M));
                }
        }

        auto any() const noexcept
        {
                return boost::algorithm::any_of(m_words, [](auto const word){
                        return word != bit::mask::none<value_type>;
                });
        }

        auto none() const noexcept
        {
                return boost::algorithm::none_of(m_words, [](auto const word){
                        return word != bit::mask::none<value_type>;
                });
        }

        auto count() const noexcept
        {
                return boost::accumulate(m_words, 0, [](auto const sum, auto const word){
                        return sum + bit::popcount(word);
                });
        }
};

template<class T, int N>
auto operator==(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return boost::equal(lhs.m_words, rhs.m_words);
}

template<class T, int N>
auto operator<(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        using boost::adaptors::reverse;
        return boost::lexicographical_compare(reverse(lhs.m_words), reverse(rhs.m_words));
}

template<class T, int N>
auto intersects(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return boost::algorithm::any_of(boost::combine(lhs.m_words, rhs.m_words), [](auto const& block){
                return boost::get<0>(block) & boost::get<1>(block);
        });
}

template<class T, int N>
auto subset_of(word_array<T, N> const& lhs, word_array<T, N> const& rhs) noexcept
{
        return boost::algorithm::all_of(boost::combine(lhs.m_words, rhs.m_words), [](auto const& block){
                return !(boost::get<0>(block) & ~boost::get<1>(block));
        });
}

}       // namespace xstd
