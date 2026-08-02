//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_INTEGER_CLASS_HPP
#define XSTD_TEST_INTEGER_CLASS_HPP

#include <xstd/type_traits.hpp> // make_unsigned_like
#include <compare>              // strong_ordering
#include <cstdint>              // int32_t, uint32_t
#include <limits>               // numeric_limits

// A signed/unsigned pair of *class* types that behave like integers: the
// shape xstd::signed_integral_like exists for and std::signed_integral can
// never describe, since no class type satisfies std::is_integral on any
// compiler or dialect. Real examples are libstdc++'s
// std::ranges::__detail::__max_diff_type and the MSVC STL's std::_Signed128;
// both are reserved-name internals, so the tests model the category with
// their own type rather than depending on either.
//
// The width is 32 bits rather than 128. What is under test is that a class
// type works at all - the operators, std::numeric_limits and
// xstd::make_unsigned_like being found through user-supplied
// specializations instead of built into the language - and none of that
// changes with the width, while a 32-bit representation keeps the expected
// values readable and the arithmetic obviously correct.
//
// Every operator computes in the unsigned representation and converts back,
// so that even the deliberately out-of-contract paths a test might reach
// (negating MIN, say) wrap rather than overflow. That is a property of this
// model, not a requirement xstd places on integer-class types.
namespace xstd::test {

class unsigned_integer_class
{
        std::uint32_t m_value{};

      public:
        unsigned_integer_class() = default;

        constexpr explicit unsigned_integer_class(std::uint32_t value) noexcept
            : m_value(value)
        {}

        // Integer-class types are constructible from *any* integral type
        // ([iterator.concept.winc]), signed ones included, and this overload
        // is what makes that true here rather than leaving int to convert
        // implicitly. It is load-bearing rather than decorative:
        // xstd::is_signed_like_v forms static_cast<T>(-1), and without a
        // signed constructor to select, that argument converts int to
        // std::uint32_t implicitly and draws -Wsign-conversion from inside
        // xstd's own header. A type modelling the category properly does not
        // put its users in that position.
        constexpr explicit unsigned_integer_class(std::int32_t value) noexcept
            : m_value(static_cast<std::uint32_t>(value))
        {}

        [[nodiscard]] constexpr explicit operator std::uint32_t() const noexcept
        {
                return m_value;
        }

        [[nodiscard]] friend constexpr auto operator==(unsigned_integer_class, unsigned_integer_class) noexcept -> bool = default;
        [[nodiscard]] friend constexpr auto operator<=>(unsigned_integer_class, unsigned_integer_class) noexcept -> std::strong_ordering = default;

        [[nodiscard]] friend constexpr auto operator-(unsigned_integer_class a) noexcept -> unsigned_integer_class
        {
                return unsigned_integer_class(0U - a.m_value);
        }

        [[nodiscard]] friend constexpr auto operator+(unsigned_integer_class a, unsigned_integer_class b) noexcept -> unsigned_integer_class
        {
                return unsigned_integer_class(a.m_value + b.m_value);
        }

        [[nodiscard]] friend constexpr auto operator-(unsigned_integer_class a, unsigned_integer_class b) noexcept -> unsigned_integer_class
        {
                return unsigned_integer_class(a.m_value - b.m_value);
        }

        [[nodiscard]] friend constexpr auto operator*(unsigned_integer_class a, unsigned_integer_class b) noexcept -> unsigned_integer_class
        {
                return unsigned_integer_class(a.m_value * b.m_value);
        }

        [[nodiscard]] friend constexpr auto operator/(unsigned_integer_class a, unsigned_integer_class b) noexcept -> unsigned_integer_class
        {
                return unsigned_integer_class(a.m_value / b.m_value);
        }

        [[nodiscard]] friend constexpr auto operator%(unsigned_integer_class a, unsigned_integer_class b) noexcept -> unsigned_integer_class
        {
                return unsigned_integer_class(a.m_value % b.m_value);
        }
};

class signed_integer_class
{
        std::int32_t m_value{};

        [[nodiscard]] constexpr auto bits() const noexcept
                -> std::uint32_t
        {
                return static_cast<std::uint32_t>(m_value);
        }

        [[nodiscard]] static constexpr auto from_bits(std::uint32_t bits) noexcept
                -> signed_integer_class
        {
                return signed_integer_class(static_cast<std::int32_t>(bits));
        }

      public:
        signed_integer_class() = default;

        constexpr explicit signed_integer_class(std::int32_t value) noexcept
            : m_value(value)
        {}

        [[nodiscard]] constexpr explicit operator std::int32_t() const noexcept
        {
                return m_value;
        }

        // the explicit conversion to its unsigned counterpart that
        // xstd::signed_integral_like requires, and that xstd::uabs runs on
        [[nodiscard]] constexpr explicit operator unsigned_integer_class() const noexcept
        {
                return unsigned_integer_class(bits());
        }

        [[nodiscard]] friend constexpr auto operator==(signed_integer_class, signed_integer_class) noexcept -> bool = default;
        [[nodiscard]] friend constexpr auto operator<=>(signed_integer_class, signed_integer_class) noexcept -> std::strong_ordering = default;

        [[nodiscard]] friend constexpr auto operator-(signed_integer_class a) noexcept -> signed_integer_class
        {
                return from_bits(0U - a.bits());
        }

        [[nodiscard]] friend constexpr auto operator+(signed_integer_class a, signed_integer_class b) noexcept -> signed_integer_class
        {
                return from_bits(a.bits() + b.bits());
        }

        [[nodiscard]] friend constexpr auto operator-(signed_integer_class a, signed_integer_class b) noexcept -> signed_integer_class
        {
                return from_bits(a.bits() - b.bits());
        }

        [[nodiscard]] friend constexpr auto operator*(signed_integer_class a, signed_integer_class b) noexcept -> signed_integer_class
        {
                return from_bits(a.bits() * b.bits());
        }

        // / and % are the two that cannot be done in the unsigned
        // representation: the signed conventions are what is under test
        [[nodiscard]] friend constexpr auto operator/(signed_integer_class a, signed_integer_class b) noexcept -> signed_integer_class
        {
                return signed_integer_class(a.m_value / b.m_value);
        }

        [[nodiscard]] friend constexpr auto operator%(signed_integer_class a, signed_integer_class b) noexcept -> signed_integer_class
        {
                return signed_integer_class(a.m_value % b.m_value);
        }
};

} // namespace xstd::test

// The user-supplied half of xstd::signed_integral_like: a *signed* class type
// has no unsigned counterpart the compiler can work out, so it names one here.
// This is the one specialization a user of a non-built-in integer type writes,
// and the reason xstd::make_unsigned_like's primary template is empty rather
// than an alias for std::make_unsigned. The unsigned type below needs none:
// like every unsigned type, it is its own counterpart, and
// xstd::make_unsigned_like says so without being told.
template<>
struct xstd::make_unsigned_like<xstd::test::signed_integer_class>
{
        using type = xstd::test::unsigned_integer_class;
};

// Inheriting from the representation's own specialization rather than
// restating it: digits, radix, is_integer, is_signed, is_exact and the rest
// are already right, and only the three that hand back a value need a type of
// the enclosing class. The float-only members (epsilon, infinity, ...) keep
// the representation's type, which is inert for a type whose is_integer is
// true and which nothing in xstd reads.
template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
struct std::numeric_limits<xstd::test::signed_integer_class> : std::numeric_limits<std::int32_t>
{
        [[nodiscard]] static constexpr auto min() noexcept -> xstd::test::signed_integer_class
        {
                return xstd::test::signed_integer_class(std::numeric_limits<std::int32_t>::min());
        }

        [[nodiscard]] static constexpr auto lowest() noexcept -> xstd::test::signed_integer_class
        {
                return min();
        }

        [[nodiscard]] static constexpr auto max() noexcept -> xstd::test::signed_integer_class
        {
                return xstd::test::signed_integer_class(std::numeric_limits<std::int32_t>::max());
        }
};

template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
struct std::numeric_limits<xstd::test::unsigned_integer_class> : std::numeric_limits<std::uint32_t>
{
        [[nodiscard]] static constexpr auto min() noexcept -> xstd::test::unsigned_integer_class
        {
                return xstd::test::unsigned_integer_class(std::numeric_limits<std::uint32_t>::min());
        }

        [[nodiscard]] static constexpr auto lowest() noexcept -> xstd::test::unsigned_integer_class
        {
                return min();
        }

        [[nodiscard]] static constexpr auto max() noexcept -> xstd::test::unsigned_integer_class
        {
                return xstd::test::unsigned_integer_class(std::numeric_limits<std::uint32_t>::max());
        }
};

#endif // XSTD_TEST_INTEGER_CLASS_HPP
