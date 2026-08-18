//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BIT_INTEGER_HPP
#define XSTD_TEST_BIT_INTEGER_HPP

#include <compare>                            // strong_ordering
#include <concepts>                           // same_as
#include <cstddef>                            // size_t
#include <limits>                             // numeric_limits

// Adapter support for compiler bit-precise integers.
namespace xstd::test {

template<class Storage>
struct storage_limits : std::numeric_limits<Storage>
{};

template<class Storage>
inline constexpr auto storage_width =
        storage_limits<Storage>::digits + static_cast<int>(storage_limits<Storage>::is_signed);

// A small adapter that gives compiler bit-precise storage the integer-class customization surface.
template<class Storage>
class bit_integer
{
        using self = bit_integer;
        using binary_result = self;

        Storage m_value{};

        // Every instantiation, so that the same-width counterpart can read this one's value.
        template<class>
        friend class bit_integer;

        // Centralized construction for binary operators.
        [[nodiscard]] static constexpr auto wrap(Storage value) -> binary_result
        {
                return from(value);
        }

      public:
        bit_integer() = default;

        // /6 one way, and not explicit: an integral value converts implicitly.
        constexpr bit_integer(int value)
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
                requires (not std::same_as<OtherStorage, Storage>) and
                         (storage_width<OtherStorage> == storage_width<Storage>)
        explicit constexpr bit_integer(bit_integer<OtherStorage> other)
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

} // namespace xstd::test

// /11's members in the type rather than the storage.
template<class Storage>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
class std::numeric_limits<xstd::test::bit_integer<Storage>>
    : public xstd::test::storage_limits<Storage>
{
        using type = xstd::test::bit_integer<Storage>;

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

#endif // XSTD_TEST_BIT_INTEGER_HPP
