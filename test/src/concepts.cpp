//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>               // integer concepts
#include <xstd/test/exact_width_types.hpp> // exact-width integer types
#include <boost/test/unit_test.hpp>        // Boost.Test

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE_TEMPLATE(SignedConceptIdentities, T, xstd::test::exact_width_signed_integer_types)
{
        static_assert(xstd::signed_integer_like<T>);
        static_assert(xstd::integer_like<T>);
        static_assert(not xstd::unsigned_integer_like<T>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedConceptIdentities, T, xstd::test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::unsigned_integer_like<T>);
        static_assert(xstd::integer_like<T>);
        static_assert(not xstd::signed_integer_like<T>);
        BOOST_CHECK(true);
}

// The narrower concept subsumes integer_like in overload resolution.
template<xstd::integer_like I>
[[nodiscard]] constexpr auto which(I) noexcept -> int
{
        return 1;
}
template<xstd::signed_integer_like S>
[[nodiscard]] constexpr auto which(S) noexcept -> int
{
        return 2;
}

BOOST_AUTO_TEST_CASE(ConceptSubsumption)
{
        static_assert(which(1) == 2);
        static_assert(which(1U) == 1);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
