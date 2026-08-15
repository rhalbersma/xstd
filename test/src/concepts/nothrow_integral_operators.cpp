//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/cstdint.hpp>                             // int128, uint128
#include <xstd/cstdlib/abs.hpp>                         // abs
#include <xstd/cstdlib/div.hpp>                         // div
#include <xstd/test/absl_int128.hpp>                    // XSTD_TEST_HAS_ABSL_INT128, absl_int128, absl_uint128
#include <xstd/test/constexpr.hpp>                      // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                      // complex

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(NothrowIntegralOperators)
{
        // The built-in widths and the 128-bit type the library names all
        // carry the specifier.
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<int>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<bool> and xstd::nothrow_integral_operators<char32_t>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<xstd::int128> and xstd::nothrow_integral_operators<xstd::uint128>);

        // An unannotated integer-class type is admitted by integral_like and
        // refused here, which is why the two are separate concepts.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<xstd::test::absl_int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<xstd::test::absl_int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<xstd::test::absl_uint128>);
#endif
}

// Cv-transparent on both branches, as integral_like is, and for the reason the
// header gives. const needs nothing: every row is stated over a const operand
// already. volatile is the one that takes the stripping, and only on the
// integer-class branch - the built-in types answer a volatile operand on the
// language's own operators, where a type whose operators are declared const
// members answers none of them. absl::uint128 is the type that shows it: the
// built-in 128-bit type reaches this concept along the integer-class branch
// too, being no longer integral in this dialect, but its operators are the
// language's rather than members, so it cannot tell the two apart.
BOOST_AUTO_TEST_CASE(NothrowIntegralOperatorsIsCvTransparentOnBothBranches)
{
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<int const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<int volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<int const volatile>);

        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<xstd::int128 const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<xstd::int128 volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integral_operators<xstd::int128 const volatile>);

        // A qualifier does not turn a no into a yes either: the unannotated
        // type is refused under every one of them.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = xstd::test::absl_int128;

        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<T const>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<T volatile>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<T const volatile>);
#endif
}

// Total, the conjunction short-circuiting on integral_like before any operator
// is named, so a caller can ask this of any type and get an answer.
BOOST_AUTO_TEST_CASE(NothrowIntegralOperatorsIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<void>);
        // The stripping is remove_cv_t rather than remove_cvref_t: a reference
        // is not an integral-like type, and making one transparent would widen
        // the domain rather than leave it alone.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<int()>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<std::complex<double>>);

        // Turned away by integral_like rather than by ~ or %, which a
        // floating-point type does not have.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<double>);
}

// The concept is the exception specification rather than a description of one,
// so the two have to agree for every element type.
BOOST_AUTO_TEST_CASE(NothrowIntegralOperatorsIsTheExceptionSpecification)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(1)) == xstd::nothrow_integral_operators<int>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(1, 1)) == xstd::nothrow_integral_operators<int>);

#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = xstd::test::absl_int128;
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(T{1})) == xstd::nothrow_integral_operators<T>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(T{1}, T{1})) == xstd::nothrow_integral_operators<T>);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
