#pragma once
#include <xstd/bit/mask.hpp>                                    // all, none, one
#include <xstd/bit/primitive.hpp>                               // popcount
#include <xstd/limits.hpp>                                      // digits
#include <xstd/type_traits.hpp>                                 // is_unsigned, is_integral, is_trivial_special_members
#include <boost/algorithm/cxx11/all_of.hpp>                     // all_of
#include <boost/algorithm/cxx11/any_of.hpp>                     // any_of
#include <boost/algorithm/cxx11/none_of.hpp>                    // none_of
#include <boost/range/adaptor/reversed.hpp>                     // reverse
#include <boost/range/algorithm/equal.hpp>                      // equal
#include <boost/range/algorithm/fill_n.hpp>                     // fill_n
#include <boost/range/algorithm/lexicographical_compare.hpp>    // lexicographical_compare
#include <boost/range/algorithm/swap_ranges.hpp>                // swap_ranges
#include <boost/range/combine.hpp>                              // combine
#include <boost/range/numeric.hpp>                              // accumulate
#include <boost/tuple/tuple.hpp>                                // get
#include <algorithm>                                            // copy_n, copy_backward
#include <cassert>                                              // assert
#include <iterator>                                             // reverse_iterator
#include <type_traits>                                          // is_pod
#include <utility>                                              // move, swap

namespace xstd {

template<class WordT, int Nw>
struct word_array
{
        static_assert(is_unsigned<WordT>{});
        static_assert(is_integral<WordT>{});
        static_assert(0 <= Nw);

        static constexpr auto static_assert_type_traits() noexcept
        {
                using T = word_array;
                static_assert(is_trivial_special_members_v<T>);
                static_assert(std::is_pod<T>{});
        }

        WordT m_words[Nw ? Nw : 1];

        // types:
        using value_type             = WordT;
        using pointer                = WordT*;
        using const_pointer          = WordT const*;
        using reference              = WordT&;
        using const_reference        = WordT const&;
        using size_type              = int;
        using difference_type        = int;
        using iterator               = pointer;
        using const_iterator         = const_pointer;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // no explicit construct/copy/destroy for aggregate type

        auto fill(value_type const& u) noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] = u;
                } else if constexpr (Nw >= 2) {
                        boost::fill_n(m_words, Nw, u);
                }
        }

        auto swap(word_array& other) noexcept
        {
                if constexpr (Nw == 1) {
                        using std::swap;
                        swap(m_words[0], other.m_words[0]);
                } else if constexpr (Nw >= 2) {
                        boost::range::swap_ranges(m_words, other.m_words);
                }
        }

        // iterators:
        constexpr auto begin()         noexcept { return       iterator(data()); }
        constexpr auto begin()   const noexcept { return const_iterator(data()); }
        constexpr auto end()           noexcept { return       iterator(data() + Nw); }
        constexpr auto end()     const noexcept { return const_iterator(data() + Nw); }
        constexpr auto rbegin()        noexcept { return       reverse_iterator(end()); }
        constexpr auto rbegin()  const noexcept { return const_reverse_iterator(end()); }
        constexpr auto rend()          noexcept { return       reverse_iterator(begin()); }
        constexpr auto rend()    const noexcept { return const_reverse_iterator(begin()); }
        constexpr auto cbegin()  const noexcept { return       iterator(data());     }
        constexpr auto cend()    const noexcept { return const_iterator(data() + Nw); }
        constexpr auto crbegin() const noexcept { return const_reverse_iterator(end());   }
        constexpr auto crend()   const noexcept { return const_reverse_iterator(begin()); }

        // capacity:
        constexpr bool      empty()     const noexcept { return Nw == 0; }
        constexpr size_type size()      const noexcept { return Nw; }
        constexpr size_type max_size()  const noexcept { return Nw; }
        constexpr size_type word_size() const noexcept { return digits<value_type>; }

        // element access:
        constexpr       reference operator[](size_type const n)       { assert(0 <= n); assert(n < Nw); return m_words[n]; }
        constexpr const_reference operator[](size_type const n) const { assert(0 <= n); assert(n < Nw); return m_words[n]; }
        constexpr       reference front()       { assert(!empty()); return m_words[0]; }
        constexpr const_reference front() const { assert(!empty()); return m_words[0]; }
        constexpr       reference back()        { assert(!empty()); return m_words[Nw ? Nw - 1 : 0]; }
        constexpr const_reference back()  const { assert(!empty()); return m_words[Nw ? Nw - 1 : 0]; }
        constexpr       pointer data()       noexcept { return &m_words[0]; }
        constexpr const_pointer data() const noexcept { return &m_words[0]; }

        constexpr auto& flip() noexcept
        {
                if constexpr (Nw == 1) {
                        m_words[0] = ~m_words[0];
                } else if constexpr (Nw >= 2) {
                        for (auto& word : m_words) {
                                word = ~word;
                        }
                }
                return *this;
        }

        constexpr auto& operator&=(word_array const& other) noexcept
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

        constexpr auto& operator|=(word_array const& other) noexcept
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

        constexpr auto& operator^=(word_array const& other) noexcept
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

        constexpr auto& operator-=(word_array const& other) noexcept
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

        auto& operator<<=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < Nw * word_size());
                if constexpr (Nw == 1) {
                        m_words[0] <<= n;
                } else if constexpr (Nw >= 2) {
                        if (n == 0) return *this;

                        auto const n_block = n / word_size();
                        auto const L_shift = n % word_size();

                        if (L_shift == 0) {
                                using std::cbegin; using std::cend; using std::end;
                                std::copy_backward(cbegin(m_words), cend(m_words) - n_block, end(m_words));
                        } else {
                                auto const R_shift = word_size() - L_shift;

                                for (auto i = Nw - 1; i > n_block; --i) {
                                        m_words[i] =
                                                (m_words[i - n_block    ] << L_shift) |
                                                (m_words[i - n_block - 1] >> R_shift)
                                        ;
                                }
                                m_words[n_block] = m_words[0] << L_shift;
                        }
                        boost::fill_n(m_words, n_block, bit::mask::none<value_type>);
                }
                return *this;
        }

        auto& operator>>=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < Nw * word_size());
                if constexpr (Nw == 1) {
                        m_words[0] >>= n;
                } else if constexpr (Nw >= 2) {
                        if (n == 0) return *this;

                        auto const n_block = n / word_size();
                        auto const R_shift = n % word_size();

                        if (R_shift == 0) {
                                using std::cbegin; using std::begin;
                                std::copy_n(cbegin(m_words) + n_block, Nw - n_block, begin(m_words));
                        } else {
                                auto const L_shift = word_size() - R_shift;

                                for (auto i = 0; i < Nw - 1 - n_block; ++i) {
                                        m_words[i] =
                                                (m_words[i + n_block    ] >> R_shift) |
                                                (m_words[i + n_block + 1] << L_shift)
                                        ;
                                }
                                m_words[Nw - 1 - n_block] = m_words[Nw - 1] >> R_shift;
                        }
                        using boost::adaptors::reversed;
                        boost::fill_n(m_words | reversed, n_block, bit::mask::none<value_type>);
                }
                return *this;
        }

        auto all() const noexcept
        {
                if constexpr (Nw == 0) {
                        return true;
                } else if constexpr (Nw == 1) {
                        return m_words[0] == bit::mask::all<value_type>;
                } else if constexpr (Nw >= 2) {
                        return boost::algorithm::all_of(m_words, [](auto const word){
                                return word == bit::mask::all<value_type>;
                        });
                }
        }

        auto any() const noexcept
        {
                if constexpr (Nw == 0) {
                        return false;
                } else if constexpr (Nw == 1) {
                        return m_words[0] != bit::mask::none<value_type>;
                } else if constexpr (Nw >= 2) {
                        return boost::algorithm::any_of(m_words, [](auto const word){
                                return word != bit::mask::none<value_type>;
                        });
                }
        }

        auto none() const noexcept
        {
                if constexpr (Nw == 0) {
                        return true;
                } else if constexpr (Nw == 1) {
                        return m_words[0] == bit::mask::none<value_type>;
                } else if constexpr (Nw >= 2) {
                        return boost::algorithm::none_of(m_words, [](auto const word){
                                return word != bit::mask::none<value_type>;
                        });
                }
        }

        auto count() const noexcept
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

        template<class HashAlgorithm>
        auto hash_append() const
        {

        }
};

template<class WordT, int Nw>
auto operator==(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        if constexpr (Nw == 0) {
                return true;
        } else if constexpr (Nw == 1) {
                return lhs.m_words[0] == rhs.m_words[0];
        } else if constexpr (Nw >= 2) {
                return boost::equal(lhs.m_words, rhs.m_words);
        }
}

template<class WordT, int Nw>
auto operator!=(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<class WordT, int Nw>
auto operator<(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        if constexpr (Nw == 0) {
                return false;
        } else if constexpr (Nw == 1) {
                return lhs.m_words[0] < rhs.m_words[0];
        } else if constexpr (Nw >= 2) {
                using boost::adaptors::reversed;
                return boost::lexicographical_compare(lhs.m_words | reversed, rhs.m_words | reversed);
        }
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

template<class WordT, int Nw>
auto intersects(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
        -> bool
{
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

template<class WordT, int Nw>
auto subset_of(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
        -> bool
{
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

template<class WordT, int Nw>
constexpr auto operator~(word_array<WordT, Nw> const& lhs) noexcept
{
        word_array<WordT, Nw> nrv{lhs}; nrv.flip(); return nrv;
}

template<class WordT, int Nw>
constexpr auto operator&(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        word_array<WordT, Nw> nrv{lhs}; nrv &= rhs; return nrv;
}

template<class WordT, int Nw>
constexpr auto operator|(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        word_array<WordT, Nw> nrv{lhs}; nrv |= rhs; return nrv;
}

template<class WordT, int Nw>
constexpr auto operator^(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        word_array<WordT, Nw> nrv{lhs}; nrv ^= rhs; return nrv;
}

template<class WordT, int Nw>
constexpr auto operator-(word_array<WordT, Nw> const& lhs, word_array<WordT, Nw> const& rhs) noexcept
{
        word_array<WordT, Nw> nrv{lhs}; nrv -= rhs; return nrv;
}

template<class WordT, int Nw>
auto operator<<(word_array<WordT, Nw> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < Nw * lhs.word_size());
        word_array<WordT, Nw> nrv{lhs}; nrv <<= n; return nrv;
}

template<class WordT, int Nw>
auto operator>>(word_array<WordT, Nw> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < Nw * lhs.word_size());
        word_array<WordT, Nw> nrv{lhs}; nrv >>= n; return nrv;
}

}       // namespace xstd
