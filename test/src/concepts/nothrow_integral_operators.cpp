//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integral_like.hpp>              // integral_like
#include <xstd/concepts/nothrow_integral_operators.hpp> // nothrow_integral_operators
#include <xstd/cstdint.hpp>                             // int128, uint128
#include <xstd/cstdlib/abs.hpp>                         // abs
#include <xstd/cstdlib/div.hpp>                         // div
#include <xstd/test/constexpr.hpp>                      // XSTD_CONSTEXPR_CHECK
#include <xstd/test/unannotated.hpp>                    // unannotated
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
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<xstd::test::unannotated>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<xstd::test::unannotated>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<xstd::test::unannotated_unsigned>);
}

// Total, the conjunction short-circuiting on integral_like before any operator
// is named, so a caller can ask this of any type and get an answer.
BOOST_AUTO_TEST_CASE(NothrowIntegralOperatorsIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integral_operators<void>);
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

        using T = xstd::test::unannotated;
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(T{1})) == xstd::nothrow_integral_operators<T>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(T{1}, T{1})) == xstd::nothrow_integral_operators<T>);
}

BOOST_AUTO_TEST_SUITE_END()
