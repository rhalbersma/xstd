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
#include <cstddef>                                 // size_t
#include <limits>                                  // numeric_limits
#include <type_traits>                             // conditional_t, type_identity

// An integer-class type that carries no noexcept anywhere. That is what
// [iterator.concept.winc] permits - the word does not occur in the subclause -
// and what absl::uint128 actually does: not one occurrence in its header,
// though it is two uint64_t halves with no way to throw.
//
// Boost.Int128 cannot stand in for this. It annotates everything, so a suite
// that only ever sees Boost.Int128 cannot tell a concept that demands the
// specifier from one that does not, and the two admit very different sets of
// types. Nothing here is noexcept and nothing here throws: those are separate
// questions, and only the first is one a declaration can be asked.
//
// Supplied as a signed/unsigned pair, because that is what a user has to supply
// for a type of their own - the two trait specializations at the bottom are the
// whole of it, exactly as for Boost.Int128.
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

        // [iterator.concept.winc]/6 grants conversions "between two integer-class
        // types" as well as to and from integral ones, which is what lets
        // unsigned_abs reach the counterpart named below.
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
// whole of what this type needs to reach the same facilities the built-in
// widths do - just as for Boost.Int128.
template<>
struct make_unsigned_like<test::unannotated> : std::type_identity<test::unannotated_unsigned>
{};

template<>
struct make_signed_like<test::unannotated_unsigned> : std::type_identity<test::unannotated>
{};

} // namespace xstd

namespace xstd::test {

// [iterator.concept.winc]/11 wants each member to have the same value as the
// corresponding member of the hypothetical extended integer type, so the ones
// naming a value have to name it in this type rather than in the storage.
// Shared by the two specializations below, which are full rather than one
// partial specialization on the signedness: clang-tidy's
// bugprone-std-namespace-modification exempts a specialization of a std
// template for a program-defined type, but does not extend that to a partial
// one.
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
