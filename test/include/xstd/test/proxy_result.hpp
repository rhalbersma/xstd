//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_PROXY_RESULT_HPP
#define XSTD_TEST_PROXY_RESULT_HPP

#include <xstd/cstdint.hpp> // uint128
#include <compare>          // strong_ordering
#include <cstddef>          // size_t
#include <limits>           // numeric_limits

// An integer-class type in everything but [iterator.concept.winc]/7.6: its
// non-assignment binary operators hand back a proxy that converts to it rather
// than it, as an expression-template arithmetic type does. /7.6 pins those
// results to I, so this is not an integer-class type, and the concept is only
// able to say so while its binary rows name the operator's own result type. A
// static_cast<I> around each of them is of type I whether or not the operator
// was, and takes this type in.
//
// A class template over its storage rather than a plain class, for a reason
// beyond the room that leaves for a second width: clang's intra-TU
// lifetime analysis does not enter a template, and every member here that
// hands back *this would otherwise want a [[clang::lifetimebound]] of its own
// under the -Weverything the test build runs with.
namespace xstd::test {

template<class Storage>
class proxy_result_type;

// What the binary operators return: convertible to the type, and not it.
template<class Storage>
class proxy_value
{
        Storage m_value;

      public:
        constexpr explicit proxy_value(Storage value)
            : m_value(value)
        {}

        [[nodiscard]] constexpr operator proxy_result_type<Storage>() const
        {
                return proxy_result_type<Storage>::from(m_value);
        }
};

template<class Storage>
class proxy_result_type
{
        using self = proxy_result_type;
        using proxy = proxy_value<Storage>;

        Storage m_value{};

      public:
        proxy_result_type() = default;

        constexpr proxy_result_type(int value)
            : m_value(static_cast<Storage>(value))
        {}

        [[nodiscard]] static constexpr auto from(Storage value) -> self
        {
                auto result = self();
                result.m_value = value;
                return result;
        }

        [[nodiscard]] explicit constexpr operator bool() const
        {
                return static_cast<bool>(m_value);
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

        // The one departure. Everything above is what the subclause asks for;
        // these ten return the proxy rather than the type.
        friend constexpr auto operator*(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value * rhs.m_value));
        }
        friend constexpr auto operator/(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value / rhs.m_value));
        }
        friend constexpr auto operator%(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value % rhs.m_value));
        }
        friend constexpr auto operator+(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value + rhs.m_value));
        }
        friend constexpr auto operator-(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value - rhs.m_value));
        }
        friend constexpr auto operator&(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value & rhs.m_value));
        }
        friend constexpr auto operator^(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value ^ rhs.m_value));
        }
        friend constexpr auto operator|(self lhs, self rhs) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value | rhs.m_value));
        }
        friend constexpr auto operator<<(self lhs, std::size_t n) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value << n));
        }
        friend constexpr auto operator>>(self lhs, std::size_t n) -> proxy
        {
                return proxy(static_cast<Storage>(lhs.m_value >> n));
        }

        friend constexpr auto operator==(self, self) -> bool = default;
        friend constexpr auto operator<=>(self, self) -> std::strong_ordering = default;
};

using proxy_result = proxy_result_type<uint128>;

} // namespace xstd::test

// /11's members in the type rather than in the storage, as for the other
// fixtures. Only is_specialized and is_integer are asked of it here.
template<>
class std::numeric_limits<xstd::test::proxy_result> : public std::numeric_limits<xstd::uint128>
{
      public:
        [[nodiscard]] static constexpr auto min() -> xstd::test::proxy_result
        {
                return xstd::test::proxy_result::from(std::numeric_limits<xstd::uint128>::min());
        }
        [[nodiscard]] static constexpr auto max() -> xstd::test::proxy_result
        {
                return xstd::test::proxy_result::from(std::numeric_limits<xstd::uint128>::max());
        }
        [[nodiscard]] static constexpr auto lowest() -> xstd::test::proxy_result
        {
                return xstd::test::proxy_result::from(std::numeric_limits<xstd::uint128>::lowest());
        }
};

#endif // XSTD_TEST_PROXY_RESULT_HPP
