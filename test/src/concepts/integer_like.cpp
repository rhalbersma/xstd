//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer_class.hpp>            // integer_class
#include <xstd/concepts/integer_class_operations.hpp> // integer_class_operations
#include <xstd/concepts/integer_like.hpp>             // integer_like
#include <xstd/concepts/signed_integer_like.hpp>      // signed_integer_like
#include <xstd/concepts/unsigned_integer_like.hpp>    // unsigned_integer_like
#include <xstd/test/absl_int128.hpp>                  // XSTD_TEST_HAS_ABSL_INT128, absl_int128, absl_uint128
#include <xstd/test/constexpr.hpp>                    // XSTD_CONSTEXPR_CHECK
#include <xstd/test/integer_class.hpp>                // conforming_int_class, conforming_signed_int_class, proxy_result, unpaired_int_class
#include <boost/test/unit_test.hpp>                   // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                                   // convertible_to, same_as

BOOST_AUTO_TEST_SUITE(Concepts)

enum class scoped : unsigned { s0 };

BOOST_AUTO_TEST_CASE(IntegralLike)
{
        // The built-in branch follows std::integral, bool included.
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<const bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<volatile bool>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<char> and xstd::integer_like<char32_t>);

        // nothing whose std::numeric_limits says it is not an integer
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<scoped>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<int*>);

        // answered rather than hard-errored, the concept short-circuiting first
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<int()>);

        // The widened concepts inherit the category traits' cv transparency.
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int const>);
}

// Disjoint branches per /3, asserted on the internal concept as the outer says yes either way.
BOOST_AUTO_TEST_CASE(NoIntegralTypeIsAnIntegerClassType)
{
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class<short>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class<char>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class<unsigned long long>);

        // Each is integer_like all the same, by the other branch.
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int> and xstd::integer_like<short>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<char> and xstd::integer_like<unsigned long long>);
}

// /7.6's result type exactly; asserted both ways, a no alone proving nothing about that clause.
BOOST_AUTO_TEST_CASE(OperatorResultsAreTheTypeItself)
{
        using conforming = xstd::test::conforming_int_class;
        using proxied = xstd::test::proxy_result;

        XSTD_CONSTEXPR_CHECK(xstd::integer_class<conforming>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class<proxied>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<conforming>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<proxied>);

        // Everything the proxy returns does convert, which is all a cast would have shown.
        XSTD_CONSTEXPR_CHECK((std::convertible_to<decltype(proxied() + proxied()), proxied>));
        XSTD_CONSTEXPR_CHECK((not std::same_as<decltype(proxied() + proxied()), proxied>));

        // absl::uint128 differs from the conforming one in nothing that matters, and agrees.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        XSTD_CONSTEXPR_CHECK(xstd::integer_class<xstd::test::absl_uint128>);
#endif
}

// The subclause states the operations; that a type come in a pair is this library's own half.
BOOST_AUTO_TEST_CASE(AnIntegerClassTypeIsOneHalfOfAPair)
{
        using unpaired = xstd::test::unpaired_int_class;
        using owned = xstd::test::conforming_int_class;
        using owned_signed = xstd::test::conforming_signed_int_class;

        // Every operation the subclause states, and no counterpart to go with them.
        XSTD_CONSTEXPR_CHECK(xstd::integer_class_operations<unpaired>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class<unpaired>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<unpaired>);

        // Its pair is registered both ways, so each half is admitted and names the other.
        XSTD_CONSTEXPR_CHECK(xstd::integer_class<owned> and xstd::integer_class<owned_signed>);
        XSTD_CONSTEXPR_CHECK(xstd::unsigned_integer_like<owned>);
        XSTD_CONSTEXPR_CHECK(xstd::signed_integer_like<owned_signed>);

        // The proxy variant fails at /7.6, so the pairing clause is never the reason.
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class_operations<xstd::test::proxy_result>);

        // The integral branch is untouched by it: std pairs those types itself.
        XSTD_CONSTEXPR_CHECK(not xstd::integer_class_operations<int>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int>);
}

// On both branches; the integer-class one takes doing, a const type failing ++a.
BOOST_AUTO_TEST_CASE(IntegralLikeIsCvTransparentOnBothBranches)
{
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int const>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int const volatile>);

        // The integer-class branch unconditionally, both third parties being optional.
        using owned = xstd::test::conforming_int_class;

        XSTD_CONSTEXPR_CHECK(xstd::integer_like<owned const>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<owned volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<owned const volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::unsigned_integer_like<owned const>);

        // The pairing clause asks about the type, not the qualifier: a user specializes once.
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<xstd::test::conforming_signed_int_class const>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<xstd::test::unpaired_int_class const>);

#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = xstd::test::absl_int128;

        XSTD_CONSTEXPR_CHECK(xstd::integer_like<T const>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<T volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<T const volatile>);

        // The signedness travels with it, on the same terms.
        XSTD_CONSTEXPR_CHECK(xstd::signed_integer_like<T const>);
        XSTD_CONSTEXPR_CHECK(xstd::unsigned_integer_like<xstd::test::absl_uint128 const>);
#endif

        // A qualifier admits nothing the unqualified type would not have been.
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<double const>);
        XSTD_CONSTEXPR_CHECK(not xstd::integer_like<scoped const>);
}

BOOST_AUTO_TEST_SUITE_END()
