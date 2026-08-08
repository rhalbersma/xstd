//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/exposition_only.hpp>        // integer_class_type
#include <xstd/concepts/integral_like.hpp>          // integral_like
#include <xstd/concepts/signed_integral_like.hpp>   // signed_integral_like
#include <xstd/concepts/unsigned_integral_like.hpp> // unsigned_integral_like
#include <xstd/test/constexpr.hpp>                  // XSTD_CONSTEXPR_CHECK
#include <xstd/test/unannotated.hpp>                // unannotated, unannotated_unsigned
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE

BOOST_AUTO_TEST_SUITE(Concepts)

enum class scoped : unsigned { s0 };

BOOST_AUTO_TEST_CASE(IntegralLike)
{
        // The built-in branch follows std::integral, including its treatment
        // of bool as an unsigned integral type.
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<char> and xstd::integral_like<char32_t>);

        // nothing whose std::numeric_limits says it is not an integer
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<scoped>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int*>);

        // the spellings that are answered rather than hard-errored only
        // because the requirements they would trip over sit inside a concept,
        // where a conjunction short-circuits. These four pin that behavior in
        // the public concept a caller actually writes.
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int()>);

        // C++23's arithmetic concepts inherit the category traits' cv
        // transparency, and the widened concepts do the same.
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int const>);
}

// Disjoint branches, per /3's width clause. Asserted on the internal concept
// because integral_like answers true for these either way.
BOOST_AUTO_TEST_CASE(NoIntegralTypeIsAnIntegerClassType)
{
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<short>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<char>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<unsigned long long>);

        // Each is integral_like all the same, by the other branch.
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int> and xstd::integral_like<short>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<char> and xstd::integral_like<unsigned long long>);
}

// On both branches. The integer-class one takes doing: its requirements are
// stated for an object that can be assigned, so a const type fails ++a.
BOOST_AUTO_TEST_CASE(IntegralLikeIsCvTransparentOnBothBranches)
{
        using T = xstd::test::unannotated;

        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int const> and xstd::integral_like<T const>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int volatile> and xstd::integral_like<T volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int const volatile> and xstd::integral_like<T const volatile>);

        // The signedness travels with it, on the same terms.
        XSTD_CONSTEXPR_CHECK(xstd::signed_integral_like<T const>);
        XSTD_CONSTEXPR_CHECK(xstd::unsigned_integral_like<xstd::test::unannotated_unsigned const>);

        // A qualifier admits nothing the unqualified type would not have been.
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<double const>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<scoped const>);
}

BOOST_AUTO_TEST_SUITE_END()
