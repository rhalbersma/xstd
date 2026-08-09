//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/exposition_only.hpp>        // integer_class_type
#include <xstd/concepts/integral_like.hpp>          // integral_like
#include <xstd/concepts/signed_integral_like.hpp>   // signed_integral_like
#include <xstd/concepts/unsigned_integral_like.hpp> // unsigned_integral_like
#include <xstd/test/constexpr.hpp>                  // XSTD_CONSTEXPR_CHECK
#include <xstd/test/proxy_result.hpp>               // proxy_result
#include <xstd/test/unannotated.hpp>                // unannotated, unannotated_unsigned
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                                 // convertible_to, same_as

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

        // answered rather than hard-errored because the requirements they
        // would trip over sit inside a concept, which short-circuits
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

// /7.3 and /7.6 give the value-producing operators a result type, and the
// concept asks for it rather than for something a static_cast<I> could reach.
// The fixture is an integer-class type in every other respect, so it is these
// rows alone that turn it away.
BOOST_AUTO_TEST_CASE(OperatorResultsAreTheTypeItself)
{
        using T = xstd::test::proxy_result;

        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<T>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<T>);

        // Everything the binary operators return does convert to it, which is
        // all a static_cast<I> in those rows would have established.
        XSTD_CONSTEXPR_CHECK((std::convertible_to<decltype(T() + T()), T>));
        XSTD_CONSTEXPR_CHECK((not std::same_as<decltype(T() + T()), T>));

        // The unannotated fixture differs from it in nothing else and passes.
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::integer_class_type<xstd::test::unannotated_unsigned>);
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
