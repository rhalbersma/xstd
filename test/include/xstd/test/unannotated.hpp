//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_UNANNOTATED_HPP
#define XSTD_TEST_UNANNOTATED_HPP

#include <xstd/cstdint.hpp>                        // int128, uint128
#include <xstd/type_traits/make_signed_like.hpp>   // make_signed_like
#include <xstd/type_traits/make_unsigned_like.hpp> // make_unsigned_like
#include <compare>                                 // strong_ordering
#include <concepts>                                // integral
#include <cstddef>                                 // size_t
#include <limits>                                  // numeric_limits
#include <type_traits>                             // conditional_t, type_identity

// An integer-class type carrying no noexcept anywhere, as absl::uint128 is and
// Boost.Int128 is not. A pair, that being what a user supplies for their own.
namespace xstd::test {

template<bool Signed>
class unannotated_type
{
        using storage = std::conditional_t<Signed, int128, uint128>;
        using self = unannotated_type;

        storage m_value{};

        struct raw
        {};
        constexpr unannotated_type(raw, storage value)
            : m_value(value)
        {}

      public:
        unannotated_type() = default;

        constexpr unannotated_type(int value)
            : m_value(static_cast<storage>(value))
        {}

        // /6 grants conversions between two integer-class types, which is what
        // lets unsigned_abs reach the counterpart named below.
        constexpr unannotated_type(unannotated_type<not Signed> other)
            : m_value(static_cast<storage>(other.value()))
        {}

        [[nodiscard]] static constexpr auto from(storage value) -> self
        {
                return {raw{}, value};
        }
        [[nodiscard]] constexpr auto value() const -> storage
        {
                return m_value;
        }

        // /6, explicitly convertible to any integer-like type, which at J =
        // bool is also /8's "as if by bool(E != I(0))".
        template<std::integral J>
        [[nodiscard]] explicit constexpr operator J() const
        {
                return static_cast<J>(m_value);
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
                return from(static_cast<storage>(-m_value));
        }
        constexpr auto operator~() const -> self
        {
                return from(static_cast<storage>(~m_value));
        }
        constexpr auto operator!() const -> bool
        {
                return not m_value;
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
        constexpr auto operator&=(self rhs) -> self&
        {
                m_value &= rhs.m_value;
                return *this;
        }
        constexpr auto operator|=(self rhs) -> self&
        {
                m_value |= rhs.m_value;
                return *this;
        }
        constexpr auto operator^=(self rhs) -> self&
        {
                m_value ^= rhs.m_value;
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

        friend constexpr auto operator+(self lhs, self rhs) -> self
        {
                return lhs += rhs;
        }
        friend constexpr auto operator-(self lhs, self rhs) -> self
        {
                return lhs -= rhs;
        }
        friend constexpr auto operator*(self lhs, self rhs) -> self
        {
                return lhs *= rhs;
        }
        friend constexpr auto operator/(self lhs, self rhs) -> self
        {
                return lhs /= rhs;
        }
        friend constexpr auto operator%(self lhs, self rhs) -> self
        {
                return lhs %= rhs;
        }
        friend constexpr auto operator&(self lhs, self rhs) -> self
        {
                return lhs &= rhs;
        }
        friend constexpr auto operator|(self lhs, self rhs) -> self
        {
                return lhs |= rhs;
        }
        friend constexpr auto operator^(self lhs, self rhs) -> self
        {
                return lhs ^= rhs;
        }
        friend constexpr auto operator<<(self lhs, std::size_t n) -> self
        {
                return lhs <<= n;
        }
        friend constexpr auto operator>>(self lhs, std::size_t n) -> self
        {
                return lhs >>= n;
        }

        friend constexpr auto operator==(self, self) -> bool = default;
        friend constexpr auto operator<=>(self, self) -> std::strong_ordering = default;
};

using unannotated = unannotated_type<true>;
using unannotated_unsigned = unannotated_type<false>;

} // namespace xstd::test

namespace xstd {

// The one thing a library cannot work out for a type it does not know, and the
// whole of what this one needs - just as for Boost.Int128.
template<>
struct make_unsigned_like<test::unannotated> : std::type_identity<test::unannotated_unsigned>
{};

template<>
struct make_signed_like<test::unannotated_unsigned> : std::type_identity<test::unannotated>
{};

} // namespace xstd

namespace xstd::test {

// /11 wants each member's value in this type rather than in the storage. Two
// full specializations rather than one partial: clang-tidy exempts only those.
template<class Self, class Storage>
class unannotated_limits : public std::numeric_limits<Storage>
{
      public:
        [[nodiscard]] static constexpr auto min() -> Self
        {
                return Self::from(std::numeric_limits<Storage>::min());
        }
        [[nodiscard]] static constexpr auto max() -> Self
        {
                return Self::from(std::numeric_limits<Storage>::max());
        }
        [[nodiscard]] static constexpr auto lowest() -> Self
        {
                return Self::from(std::numeric_limits<Storage>::lowest());
        }
};

} // namespace xstd::test

template<>
class std::numeric_limits<xstd::test::unannotated>
    : public xstd::test::unannotated_limits<xstd::test::unannotated, xstd::int128>
{};

template<>
class std::numeric_limits<xstd::test::unannotated_unsigned>
    : public xstd::test::unannotated_limits<xstd::test::unannotated_unsigned, xstd::uint128>
{};

#endif // XSTD_TEST_UNANNOTATED_HPP
