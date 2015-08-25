#pragma once
#include <xstd/bit/base_bitset.hpp>     // base_bitset
#include <xstd/bit/mask.hpp>            // one
#include <xstd/bit/proxy.hpp>           // ConstIterator, ConstReference
#include <xstd/bit/input_range.hpp>
#include <xstd/limits.hpp>              // digits
#include <cassert>                      // assert
#include <initializer_list>             // initializer_list
#include <iterator>                     // reverse_iterator
#include <type_traits>                  // enable_if_t

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
        public bit::detail::base_bitset<unsigned long long, num_blocks<unsigned long long>(N)>
{
        using block_type = unsigned long long;
        static constexpr auto Nb = num_blocks<block_type>(N);
        using Base = bit::detail::base_bitset<block_type, num_blocks<block_type>(N)>;

        friend constexpr bool operator== <>(const bitset<N>&, const bitset<N>&) noexcept;
        friend constexpr bool operator<  <>(const bitset<N>&, const bitset<N>&) noexcept;
        friend constexpr bool intersect  <>(const bitset<N>&, const bitset<N>&) noexcept;
public:
        using reference = bit::ConstReference<block_type, Nb, N>;
        using const_reference = reference;
        using iterator = bit::ConstIterator<block_type, Nb, N>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // constructors

        constexpr bitset() = default;

        template<class ForwardIterator>
        constexpr bitset(ForwardIterator first, ForwardIterator last)
        :
                Base{}
        {
                for (auto it = first; it != last; ++it)
                        set(*it);
        }

        constexpr bitset(std::initializer_list<std::size_t> ilist)
        :
                bitset(ilist.begin(), ilist.end())
        {}

        [[deprecated]] /* implicit */ constexpr bitset(unsigned long long value) noexcept
        :
                Base{sanitized(value)}
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

        constexpr auto end() noexcept
        {
                return iterator{this->block_end(), N};
        }

        constexpr auto end() const noexcept
        {
                return const_iterator{this->block_end(), N};
        }

        constexpr auto rbegin() noexcept
        {
                return reverse_iterator{end()};
        }

        constexpr auto rbegin() const noexcept
        {
                return const_reverse_iterator{end()};
        }

        constexpr auto rend() noexcept
        {
                return reverse_iterator{begin()};
        }

        constexpr auto rend() const noexcept
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

        constexpr auto crbegin() const noexcept
        {
                return rbegin();
        }

        constexpr auto crend() const noexcept
        {
                return rend();
        }

        // capacity

        static constexpr std::size_t size() noexcept
        {
                return N;
        }

        // modifiers

        constexpr void swap(bitset<N>& other) noexcept
        {
                this->do_swap(other);
        }

        // element access

        constexpr bool test(std::size_t pos) const // Throws: Nothing.
        {
                assert(pos < N);
                return this->block_ref(pos) & this->block_mask(pos);
        }

        constexpr bitset<N>& set(std::size_t pos) // Throws: Nothing.
        {
                assert(pos < N);
                this->block_ref(pos) |= this->block_mask(pos);
                assert(test(pos));
                return *this;
        }

        constexpr bitset<N>& reset(std::size_t pos) // Throws: Nothing.
        {
                assert(pos < N);
                this->block_ref(pos) &= ~this->block_mask(pos);
                assert(!test(pos));
                return *this;
        }

        constexpr bitset<N>& flip(std::size_t pos) // Throws: Nothing.
        {
                assert(pos < N);
                this->block_ref(pos) ^= this->block_mask(pos);
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

        constexpr bool is_subset_of(const bitset<N>& other) const noexcept
        {
                return this->do_is_subset_of(other);
        }

        constexpr bool is_superset_of(const bitset<N>& other) const noexcept
        {
                return other.is_subset_of(*this);
        }

        constexpr bool is_proper_subset_of(const bitset<N>& other) const noexcept
        {
                return this->do_is_subset_of(other) && !other.is_subset_of(*this);
        }

        constexpr bool is_proper_superset_of(const bitset<N>& other) const noexcept
        {
                return this->do_is_superset_of(other) && !other.is_superset_of(*this);
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
                this->op_flip();
                sanitize();
                return *this;
        }

        constexpr bitset<N>& operator&=(const bitset<N>& rhs) noexcept
        {
                this->op_and(rhs);
                return *this;
        }

        constexpr bitset<N>& operator|=(const bitset<N>& rhs) noexcept
        {
                this->op_or(rhs);
                return *this;
        }

        constexpr bitset<N>& operator^=(const bitset<N>& rhs) noexcept
        {
                this->op_xor(rhs);
                return *this;
        }

        constexpr bitset<N>& operator-=(const bitset<N>& rhs) noexcept
        {
                this->op_minus(rhs);
                return *this;
        }

        constexpr bitset<N>& operator<<=(std::size_t pos) // Throws: Nothing.
        {
                assert(pos < N);
                this->op_left_shift(pos);
                sanitize();
                return *this;
        }

        constexpr bitset<N>& operator>>=(std::size_t pos) // Throws: Nothing.
        {
                assert(pos < N);
                this->op_right_shift(pos);
                return *this;
        }

        template<class UnaryFunction>
        constexpr auto consume_each(UnaryFunction f)
        {
                this->do_consume_each(f);
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
        constexpr auto sanitized(block_type b) const noexcept
        {
                return sanitized<N % digits<block_type>>(b);
        }

        constexpr auto sanitize() noexcept
        {
                sanitize<N % digits<block_type>>(this->block_back());
        }

        template<std::size_t M>
        constexpr auto sanitized(block_type b) noexcept
                -> std::enable_if_t<M != 0, block_type>
        {
                static_assert(M < digits<block_type>, "");
                return b & (bit::mask::all<block_type> >> (digits<block_type> - M));
        }

        template<std::size_t M>
        constexpr auto sanitized(block_type b) noexcept
                -> std::enable_if_t<M == 0, block_type>
        {
                return b;
        }

        template<std::size_t M>
        constexpr auto sanitize(block_type& b) noexcept
                -> std::enable_if_t<M != 0, void>
        {
                static_assert(M < digits<block_type>, "");
                b &= bit::mask::all<block_type> >> (digits<block_type> - M);
        }

        template<std::size_t M>
        constexpr auto sanitize(block_type& /* b */) noexcept
                -> std::enable_if_t<M == 0, void>
        {
                // no-op
        }
};

template<std::size_t N>
constexpr auto
begin(bitset<N>& b) -> decltype(b.begin())
{
        return b.begin();
}

template<std::size_t N>
constexpr auto
begin(const bitset<N>& b) -> decltype(b.begin())
{
        return b.begin();
}

template<std::size_t N>
constexpr auto
end(bitset<N>& b) -> decltype(b.end())
{
        return b.end();
}

template<std::size_t N>
constexpr auto
end(const bitset<N>& b) -> decltype(b.end())
{
        return b.end();
}

template<std::size_t N>
constexpr auto
cbegin(const bitset<N>& b) noexcept(noexcept(std::begin(b))) -> decltype(xstd::begin(b))
{
        return xstd::begin(b);
}

template<std::size_t N>
constexpr auto
cend(const bitset<N>& b) noexcept(noexcept(std::end(b)))-> decltype(xstd::end(b))
{
        return xstd::end(b);
}

template<std::size_t N>
constexpr auto
rbegin(bitset<N>& b) -> decltype(b.rbegin())
{
        return b.rbegin();
}

template<std::size_t N>
constexpr auto
rbegin(const bitset<N>& b) -> decltype(b.rbegin())
{
        return b.rbegin();
}

template<std::size_t N>
constexpr auto
rend(bitset<N>& b) -> decltype(b.rend())
{
        return b.rend();
}

template<std::size_t N>
constexpr auto
rend(const bitset<N>& b) -> decltype(b.rend())
{
        return b.rend();
}

template<std::size_t N>
constexpr auto
crbegin(const bitset<N>& b) -> decltype(xstd::rbegin(b))
{
        return xstd::rbegin(b);
}

template<std::size_t N>
constexpr auto
crend(const bitset<N>& b) -> decltype(xstd::rend(b))
{
        return xstd::rend(b);
}

template<std::size_t N>
constexpr bool operator==(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return lhs.op_equal_to(rhs);
}

template<std::size_t N>
constexpr bool operator<(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return lhs.op_less(rhs);
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
constexpr void swap(bitset<N>& lhs, bitset<N>& rhs) noexcept
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
        bitset<N> nrv{lhs}; nrv.flip(); return nrv;
}

template<std::size_t N>
constexpr bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        bitset<N> nrv{lhs}; nrv &= rhs; return nrv;
}

template<std::size_t N>
constexpr bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        bitset<N> nrv{lhs}; nrv |= rhs; return nrv;
}

template<std::size_t N>
constexpr bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        bitset<N> nrv{lhs}; nrv ^= rhs; return nrv;
}

template<std::size_t N>
constexpr bitset<N> operator-(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        bitset<N> nrv{lhs}; nrv -= rhs; return nrv;
}

template<std::size_t N>
constexpr bitset<N> operator<<(const bitset<N>& lhs, std::size_t pos) // Throws: Nothing.
{
        assert(pos < N);
        bitset<N> nrv{lhs}; nrv <<= pos; return nrv;
}

template<std::size_t N>
constexpr bitset<N> operator>>(const bitset<N>& lhs, std::size_t pos) // Throws: Nothing.
{
        assert(pos < N);
        bitset<N> nrv{lhs}; nrv >>= pos; return nrv;
}

}       // namespace xstd
