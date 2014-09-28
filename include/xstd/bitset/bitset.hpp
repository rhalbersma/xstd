#pragma once
#include <xstd/bitset/detail/base_bitset.hpp>   // base_bitset
#include <xstd/bitset/iterator/iterator.hpp>    // ConstIterator
#include <xstd/bitset/iterator/reference.hpp>   // ConstReference
#include <xstd/bitset/limits.hpp>               // digits
#include <xstd/bitset/masks.hpp>                // one
#include <cassert>                              // assert
#include <initializer_list>                     // initializer_list
#include <iterator>                             // reverse_iterator
#include <type_traits>                          // enable_if_t

namespace xstd {

template<class Block>
constexpr auto num_blocks(std::size_t N)
{
        return (N - 1 + digits<Block>) / digits<Block>;
}

template<std::size_t>
class bitset;

template<std::size_t N>
constexpr bool operator==(const bitset<N>& lhs, const bitset<N>& rhs) noexcept;

template<std::size_t N>
constexpr bool operator<(const bitset<N>& lhs, const bitset<N>& rhs) noexcept;

template<std::size_t N>
constexpr bool intersect(const bitset<N>& lhs, const bitset<N>& rhs) noexcept;

template<std::size_t N>
class bitset
:
        private detail::base_bitset<unsigned long long, num_blocks<unsigned long long>(N)>
{
        using block_type = unsigned long long;
        static constexpr auto Nb = num_blocks<block_type>(N);
        using Base = detail::base_bitset<unsigned long long, num_blocks<unsigned long long>(N)>;

public:
        using reference = ConstReference<block_type, Nb, N>;
        using const_reference = reference;
        using iterator = ConstIterator<block_type, Nb, N>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // constructors

        constexpr bitset() = default;

        /* implicit */ constexpr bitset(unsigned long long value) noexcept
        :
                Base{sanitized(value)}
        {}

        template<class ForwardIterator>
        constexpr bitset(ForwardIterator first, ForwardIterator last)
        {
                for (auto it = first; it != last; ++it)
                        set(*it);
        }

        constexpr bitset(std::initializer_list<std::size_t> ilist)
        :
                bitset(ilist.begin(), ilist.end())
        {}

        // iterators

        constexpr auto begin() noexcept
        {
                return iterator{this->block_begin()};
        }

        constexpr auto begin() const noexcept
        {
                return const_iterator{this->block_begin()};
        }

        constexpr iterator end() noexcept
        {
                return { this->block_end(), N };
        }

        constexpr const_iterator end() const noexcept
        {
                return { this->block_end(), N };
        }

        /* constexpr */ auto rbegin() noexcept
        {
                return reverse_iterator{end()};
        }

        /* constexpr */ auto rbegin() const noexcept
        {
                return const_reverse_iterator{end()};
        }

        /* constexpr */ auto rend() noexcept
        {
                return reverse_iterator{begin()};
        }

        /* constexpr */ auto rend() const noexcept
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

        /* constexpr */ auto crbegin() const noexcept
        {
                return rbegin();
        }

        /* constexpr */ auto crend() const noexcept
        {
                return rend();
        }

        // capacity

        static constexpr std::size_t size() noexcept
        {
                return N;
        }

        // modifiers

        /* constexpr */ void swap(bitset<N>& other) noexcept
        {
                this->do_swap(other);
        }

        // element access

        [[deprecated]] constexpr reference operator[](std::size_t pos)
        {
                assert(0 <= pos && pos < N);
                return { this->block_ref(pos), pos };
        }

        [[deprecated]] constexpr bool operator[](std::size_t pos) const
        {
                assert(0 <= pos && pos < N);
                return test(pos);
        }

        constexpr bool test(std::size_t pos) const
        {
                assert(0 <= pos && pos < N);
                return this->block_ref(pos) & mask(pos);
        }

        [[deprecated]] constexpr bitset<N>& set(std::size_t pos, bool value)
        {
                return value ? set(pos) : reset(pos);
        }

        constexpr bitset<N>& set(std::size_t pos)
        {
                assert(0 <= pos && pos < N);
                this->block_ref(pos) |= mask(pos);
                assert(test(pos));
                return *this;
        }

        constexpr bitset<N>& reset(std::size_t pos)
        {
                assert(0 <= pos && pos < N);
                this->block_ref(pos) &= ~mask(pos);
                assert(!test(pos));
                return *this;
        }

        constexpr bitset<N>& flip(std::size_t pos)
        {
                assert(0 <= pos && pos < N);
                this->block_ref(pos) ^= mask(pos);
                return *this;
        }

        constexpr auto* data() noexcept
        {
                return this->block_begin();
        }

        constexpr auto const* data() const noexcept
        {
                return this->block_begin();
        }

        // comparators

        friend constexpr bool operator== <>(bitset<N> const&, bitset<N> const&) noexcept;
        friend constexpr bool operator<  <>(bitset<N> const&, bitset<N> const&) noexcept;
        friend constexpr bool intersect  <>(bitset<N> const&, bitset<N> const&) noexcept;

        constexpr bool is_subset_of(bitset<N> const& other) const noexcept
        {
                return this->do_is_subset_of(other);
        }

        constexpr bool is_superset_of(bitset<N> const& other) const noexcept
        {
                return other.is_subset_of(*this);
        }

        constexpr bool is_proper_subset_of(bitset<N> const& other) const noexcept
        {
                return this->do_is_proper_subset_of(other);
        }

        constexpr bool is_proper_superset_of(bitset<N> const& other) const noexcept
        {
                return other.is_proper_subset_of(*this);
        }

        // modifiers

        constexpr bitset<N>& set() noexcept
        {
                this->do_set();
                sanitize();
                assert(all());
                return *this;
        }

        constexpr bitset<N>& reset() noexcept
        {
                this->do_reset();
                assert(none());
                return *this;
        }

        constexpr bitset<N>& flip() noexcept
        {
                this->do_flip();
                sanitize();
                return *this;
        }

        constexpr bitset<N>& operator&=(bitset<N> const& rhs) noexcept
        {
                this->do_and(rhs);
                return *this;
        }

        constexpr bitset<N>& operator|=(bitset<N> const& rhs) noexcept
        {
                this->do_or(rhs);
                return *this;
        }

        constexpr bitset<N>& operator^=(bitset<N> const& rhs) noexcept
        {
                this->do_xor(rhs);
                return *this;
        }

        constexpr bitset<N>& operator-=(bitset<N> const& rhs) noexcept
        {
                this->do_minus(rhs);
                return *this;
        }

        constexpr bitset<N>& operator<<=(std::size_t pos)
        {
                assert(0 <= pos && pos < N);
                this->do_left_shift(pos);
                sanitize();
                return *this;
        }

        constexpr bitset<N>& operator>>=(std::size_t pos)
        {
                assert(0 <= pos && pos < N);
                this->do_right_shift(pos);
                return *this;
        }

        // observers

        constexpr bool all() const noexcept
        {
                return this->template do_all<N % digits<block_type>>();
        }

        constexpr bool any() const noexcept
        {
                return this->do_any();
        }

        constexpr bool none() const noexcept
        {
                return this->do_none();
        }

        constexpr std::size_t count() const noexcept
        {
                return this->do_count();
        }

private:
        constexpr auto mask(std::size_t pos) const noexcept
        {
                return masks::one<block_type> << (pos % digits<block_type>);
        }

        constexpr auto sanitized(block_type b) const noexcept
        {
                return sanitized<N % digits<block_type>>(b);
        }

        constexpr auto sanitize() noexcept
        {
                sanitize<N % digits<block_type>>(this->block_back());
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M != 0,
        block_type> sanitized(block_type b) noexcept
        {
                static_assert(0 < M && M < digits<block_type>, "");
                return b & (masks::all<block_type> >> (digits<block_type> - M));
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M == 0,
        block_type> sanitized(block_type b) noexcept
        {
                return b;
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M != 0,
        void> sanitize(block_type& b) noexcept
        {
                static_assert(0 < M && M < digits<block_type>, "");
                b &= masks::all<block_type> >> (digits<block_type> - M);
        }

        template<std::size_t M>
        constexpr std::enable_if_t<M == 0,
        void> sanitize(block_type& /* b */) noexcept
        {
                // no-op
        }
};

template<std::size_t N>
constexpr bool operator==(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return lhs.do_equal(rhs);
}

template<std::size_t N>
constexpr bool operator<(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return lhs.do_colexicographical_compare(rhs);
}

template<std::size_t N>
constexpr bool operator!=(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return !(lhs == rhs);
}

template<std::size_t N>
constexpr bool operator>(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return rhs < lhs;
}

template<std::size_t N>
constexpr bool operator>=(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return !(lhs < rhs);
}

template<std::size_t N>
constexpr bool operator<=(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return !(rhs < lhs);
}

template<std::size_t N>
/* constexpr */ void swap(bitset<N>& lhs, bitset<N>& rhs) noexcept
{
        lhs.swap(rhs);
}

template<std::size_t N>
constexpr bool intersect(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return lhs.do_intersects(rhs);
}

template<std::size_t N>
constexpr bool disjoint(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return !intersect(lhs, rhs);
}

template<std::size_t N>
constexpr bitset<N> operator~(const bitset<N>& lhs) noexcept
{
        return bitset<N>(lhs).flip();
}

template<std::size_t N>
constexpr bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return bitset<N>(lhs) &= rhs;
}

template<std::size_t N>
constexpr bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return bitset<N>(lhs) |= rhs;
}

template<std::size_t N>
constexpr bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return bitset<N>(lhs) ^= rhs;
}

template<std::size_t N>
constexpr bitset<N> operator-(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return bitset<N>(lhs) -= rhs;
}

template<std::size_t N>
constexpr bitset<N> operator<<(const bitset<N>& lhs, std::size_t pos)
{
        assert(0 <= pos && pos < N);
        return bitset<N>(lhs) <<= pos;
}

template<std::size_t N>
constexpr bitset<N> operator>>(const bitset<N>& lhs, std::size_t pos)
{
        assert(0 <= pos && pos < N);
        return bitset<N>(lhs) >>= pos;
}

template<std::size_t N>
constexpr auto begin(const bitset<N>& s) noexcept
{
        return s.begin();
}

template<std::size_t N>
constexpr auto end(const bitset<N>& s) noexcept
{
        return s.end();
}

template<std::size_t N>
constexpr auto cbegin(const bitset<N>& s) noexcept
{
        return begin(s);
}

template<std::size_t N>
constexpr auto cend(const bitset<N>& s) noexcept
{
        return end(s);
}

template<std::size_t N>
/* constexpr */ auto rbegin(const bitset<N>& s) noexcept
{
        return s.rbegin();
}

template<std::size_t N>
/* constexpr */ auto rend(const bitset<N>& s) noexcept
{
        return s.rend();
}

template<std::size_t N>
/* constexpr */ auto crbegin(const bitset<N>& s) noexcept
{
        return rbegin(s);
}

template<std::size_t N>
/* constexpr */ auto crend(const bitset<N>& s) noexcept
{
        return rend(s);
}

}       // namespace xstd
