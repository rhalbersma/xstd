//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/nothrow_regular.hpp> // nothrow_regular
#include <xstd/ints/cstdint.hpp>                  // XSTD_HAS_BIT_INT, bit_int, bit_uint, int128, uint128
#include <xstd/test/exact_width_types.hpp>        // exact_width_integer_types
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <compare>                                // strong_ordering
#include <concepts>                               // regular
#include <cstddef>                                // size_t
#include <type_traits>                            // is_nothrow_..._v

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(NothrowRegular)

BOOST_AUTO_TEST_CASE_TEMPLATE(HoldsForEveryConfiguredWidth, T, xstd::test::exact_width_integer_types)
{
        static_assert(xstd::nothrow_regular<T>);
        BOOST_CHECK(true);
}

// Cv-transparent, as every concept in this directory is.
BOOST_AUTO_TEST_CASE(IsCvTransparent)
{
        static_assert(xstd::nothrow_regular<int const>);
        static_assert(xstd::nothrow_regular<int volatile>);
        static_assert(xstd::nothrow_regular<int const volatile>);
        static_assert(xstd::nothrow_regular<xstd::uint128 const volatile>);
        BOOST_CHECK(true);
}

// Total, like the concepts it sits beside: a non-object type is turned away by
// std::regular rather than by naming one of the traits of a type that has none.
BOOST_AUTO_TEST_CASE(IsTotal)
{
        static_assert(not xstd::nothrow_regular<void>);
        static_assert(not xstd::nothrow_regular<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        static_assert(not xstd::nothrow_regular<int[3]>);
        static_assert(not xstd::nothrow_regular<int()>);
        BOOST_CHECK(true);
}

// Regular, and noexcept on every operator, but not on the way in or across.
// The case this concept exists for: nothing else in the directory looks there.
struct leaky
{
        std::size_t v = 0;

        // Not = default: the throwing default constructor is the point of the fixture,
        // and a defaulted one would be noexcept.
        leaky() noexcept(false)
            : v(1)
        {}

        leaky(leaky const&) noexcept = default;
        leaky(leaky&&) noexcept = default;
        ~leaky() = default;

        explicit leaky(std::size_t n) noexcept
            : v(n)
        {}

        // NOLINTNEXTLINE(modernize-use-equals-default): a defaulted assignment would be noexcept
        auto operator=(leaky const& other) noexcept(false) -> leaky&
        {
                v = other.v;
                return *this;
        }

        // NOLINTNEXTLINE(modernize-use-equals-default): as above
        auto operator=(leaky&& other) noexcept(false) -> leaky&
        {
                v = other.v;
                return *this;
        }

        [[nodiscard]] auto operator<=>(leaky const&) const noexcept -> std::strong_ordering = default;
        [[nodiscard]] auto operator==(leaky const&) const noexcept -> bool = default;
};

BOOST_AUTO_TEST_CASE(SeparatesRegularityFromItsCost)
{
        // Regular, so integer_class's /9 would be satisfied.
        static_assert(std::regular<leaky>);

        // And free in neither direction, which is what this concept is here to see.
        static_assert(not std::is_nothrow_default_constructible_v<leaky>);
        static_assert(not std::is_nothrow_copy_assignable_v<leaky>);
        static_assert(not std::is_nothrow_move_assignable_v<leaky>);
        static_assert(not std::is_nothrow_swappable_v<leaky>);
        static_assert(not xstd::nothrow_regular<leaky>);

        // The halves it does carry, so the no above is not a no to everything.
        static_assert(std::is_nothrow_destructible_v<leaky>);
        static_assert(std::is_nothrow_copy_constructible_v<leaky>);

        // And it behaves, so the fixture is a type and not just a set of annotations.
        auto a = leaky{};
        auto const b = leaky{7UZ};
        BOOST_CHECK(a == leaky{});
        BOOST_CHECK(a != b);
        a = b;
        BOOST_CHECK(a == b);
        BOOST_CHECK(a >= b and a <= b);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
