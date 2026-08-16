//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_INTEGER_CLASS_HPP
#define XSTD_TEST_INTEGER_CLASS_HPP

#include <xstd/cstdint.hpp> // int128, uint128
#include <compare>          // strong_ordering
#include <cstddef>          // size_t
#include <limits>           // numeric_limits
#include <type_traits>      // conditional_t, is_same_v

// One integer-class type in two variants, differing at /7.6 alone so a case can assert both.
namespace xstd::test {

template<class Storage, bool ProxyResults>
class integer_class;

// The storage's limits, an indirection so a storage std may not describe can be described here.
template<class Storage>
struct storage_limits : std::numeric_limits<Storage>
{};

// Sign bit included, so that a counterpart is the same width rather than the same digits.
template<class Storage>
inline constexpr auto storage_width = storage_limits<Storage>::digits + storage_limits<Storage>::is_signed;

// What the binary operators return in the proxy variant: convertible to the type, not it.
template<class Storage, bool ProxyResults>
class proxy_value
{
        Storage m_value;

      public:
        constexpr explicit proxy_value(Storage value)
            : m_value(value)
        {}

        [[nodiscard]] constexpr operator integer_class<Storage, ProxyResults>() const
        {
                return integer_class<Storage, ProxyResults>::from(m_value);
        }
};

template<class Storage, bool ProxyResults>
class integer_class
{
        using self = integer_class;
        using binary_result = std::conditional_t<ProxyResults, proxy_value<Storage, ProxyResults>, self>;

        Storage m_value{};

        // Every instantiation, so that the same-width counterpart can read this one's value.
        template<class, bool>
        friend class integer_class;

        // The one line the two variants differ on, reached by every binary operator below.
        [[nodiscard]] static constexpr auto wrap(Storage value) -> binary_result
        {
                if constexpr (ProxyResults) {
                        return proxy_value<Storage, ProxyResults>(value);
                } else {
                        return from(value);
                }
        }

      public:
        integer_class() = default;

        // /6 one way, and not explicit: an integral value converts implicitly.
        constexpr integer_class(int value)
            : m_value(static_cast<Storage>(value))
        {}

        [[nodiscard]] static constexpr auto from(Storage value) -> self
        {
                auto result = self();
                result.m_value = value;
                return result;
        }

        // The conversion unsigned_abs and to_chars perform on the counterpart they return.
        template<class OtherStorage>
                requires (not std::is_same_v<OtherStorage, Storage>) and
                         (storage_width<OtherStorage> == storage_width<Storage>)
        explicit constexpr integer_class(integer_class<OtherStorage, ProxyResults> other)
            : m_value(static_cast<Storage>(other.m_value))
        {}

        // /8, and /6 the other way: explicit conversions out to the integral types.
        [[nodiscard]] explicit constexpr operator bool() const
        {
                return static_cast<bool>(m_value);
        }
        [[nodiscard]] explicit constexpr operator int() const
        {
                return static_cast<int>(m_value);
        }
        [[nodiscard]] explicit constexpr operator std::size_t() const
        {
                return static_cast<std::size_t>(m_value);
        }

        constexpr auto operator++() -> self&
        {
                ++m_value;
                return *this;
        }
        constexpr auto operator--() -> self&
        {
                --m_value;
                return *this;
        }
        constexpr auto operator++(int) -> self
        {
                auto copy = *this;
                ++m_value;
                return copy;
        }
        constexpr auto operator--(int) -> self
        {
                auto copy = *this;
                --m_value;
                return copy;
        }

        constexpr auto operator+() const -> self
        {
                return *this;
        }
        constexpr auto operator-() const -> self
        {
                return from(static_cast<Storage>(-m_value));
        }
        constexpr auto operator~() const -> self
        {
                return from(static_cast<Storage>(~m_value));
        }
        constexpr auto operator!() const -> bool
        {
                return not m_value;
        }

        constexpr auto operator*=(self rhs) -> self&
        {
                m_value *= rhs.m_value;
                return *this;
        }
        constexpr auto operator/=(self rhs) -> self&
        {
                m_value /= rhs.m_value;
                return *this;
        }
        constexpr auto operator%=(self rhs) -> self&
        {
                m_value %= rhs.m_value;
                return *this;
        }
        constexpr auto operator+=(self rhs) -> self&
        {
                m_value += rhs.m_value;
                return *this;
        }
        constexpr auto operator-=(self rhs) -> self&
        {
                m_value -= rhs.m_value;
                return *this;
        }
        constexpr auto operator&=(self rhs) -> self&
        {
                m_value &= rhs.m_value;
                return *this;
        }
        constexpr auto operator^=(self rhs) -> self&
        {
                m_value ^= rhs.m_value;
                return *this;
        }
        constexpr auto operator|=(self rhs) -> self&
        {
                m_value |= rhs.m_value;
                return *this;
        }
        constexpr auto operator<<=(std::size_t n) -> self&
        {
                m_value <<= n;
                return *this;
        }
        constexpr auto operator>>=(std::size_t n) -> self&
        {
                m_value >>= n;
                return *this;
        }

        // /7.6, and the ten the variants disagree about: self, or a proxy convertible to it.
        friend constexpr auto operator*(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value * rhs.m_value));
        }
        friend constexpr auto operator/(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value / rhs.m_value));
        }
        friend constexpr auto operator%(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value % rhs.m_value));
        }
        friend constexpr auto operator+(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value + rhs.m_value));
        }
        friend constexpr auto operator-(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value - rhs.m_value));
        }
        friend constexpr auto operator&(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value & rhs.m_value));
        }
        friend constexpr auto operator^(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value ^ rhs.m_value));
        }
        friend constexpr auto operator|(self lhs, self rhs) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value | rhs.m_value));
        }
        friend constexpr auto operator<<(self lhs, std::size_t n) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value << n));
        }
        friend constexpr auto operator>>(self lhs, std::size_t n) -> binary_result
        {
                return wrap(static_cast<Storage>(lhs.m_value >> n));
        }

        // Written out, not defaulted: a bit-precise member's implied comparison is ambiguous.
        friend constexpr auto operator==(self lhs, self rhs) -> bool
        {
                return lhs.m_value == rhs.m_value;
        }
        friend constexpr auto operator<=>(self lhs, self rhs) -> std::strong_ordering
        {
                return lhs.m_value <=> rhs.m_value;
        }
};

// Conforming, and its own unsigned counterpart by make_unsigned_like's partial specialization.
using conforming_int_class = integer_class<uint128, false>;

// The same type with /7.6 broken, and nothing else changed.
using proxy_result = integer_class<uint128, true>;

// Conforming and signed, with the counterpart unregistered that unsigned_abs would return.
using unregistered_int_class = integer_class<int128, false>;

} // namespace xstd::test

// /11's members in the type rather than the storage.
template<class Storage, bool ProxyResults>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
class std::numeric_limits<xstd::test::integer_class<Storage, ProxyResults>>
    : public xstd::test::storage_limits<Storage>
{
        using type = xstd::test::integer_class<Storage, ProxyResults>;

      public:
        [[nodiscard]] static constexpr auto min() -> type
        {
                return type::from(xstd::test::storage_limits<Storage>::min());
        }
        [[nodiscard]] static constexpr auto max() -> type
        {
                return type::from(xstd::test::storage_limits<Storage>::max());
        }
        [[nodiscard]] static constexpr auto lowest() -> type
        {
                return type::from(xstd::test::storage_limits<Storage>::lowest());
        }
};

#endif // XSTD_TEST_INTEGER_CLASS_HPP
