//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer.hpp>                   // NOLINT(misc-include-cleaner): used with optional Abseil fixtures
#include <xstd/concepts/nothrow_integer_operators.hpp> // nothrow_integer_operators
#include <xstd/cstdint.hpp>                            // int128, uint128
#include <xstd/cstdlib/abs.hpp>                        // abs
#include <xstd/cstdlib/div.hpp>                        // div
#include <xstd/test/absl_int128.hpp>                   // NOLINT(misc-include-cleaner): conditionally supplies the optional fixtures
#include <xstd/test/constexpr_check.hpp>               // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                     // complex

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(NothrowIntegerOperators)
{
        // The built-in widths and the 128-bit type the library names all carry it.
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<char32_t>);

        // Its first conjunct is integer, which cv bool is not, so the operators go unasked.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<xstd::int128> and xstd::nothrow_integer_operators<xstd::uint128>);

#ifdef XSTD_HAS_BIT_INT
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<xstd::bit_int<17>>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<xstd::bit_uint<17>>);
#endif

        // Admitted by integer and refused here: why the two are separate concepts.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        XSTD_CONSTEXPR_CHECK(xstd::integer<xstd::test::absl_int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<xstd::test::absl_int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<xstd::test::absl_uint128>);
#endif
}

// Cv-transparent on both branches; volatile is the one that takes the stripping.
BOOST_AUTO_TEST_CASE(NothrowIntegerOperatorsIsCvTransparentOnBothBranches)
{
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<int const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<int volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<int const volatile>);

        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<xstd::int128 const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<xstd::int128 volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_integer_operators<xstd::int128 const volatile>);

        // Nor does a qualifier turn a no into a yes.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = xstd::test::absl_int128;

        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<T const>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<T volatile>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<T const volatile>);
#endif
}

// Total: the conjunction short-circuits on integer before any operator is named.
BOOST_AUTO_TEST_CASE(NothrowIntegerOperatorsIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<void>);
        // remove_cv_t, not remove_cvref_t: a reference is not an integer type.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<int()>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<std::complex<double>>);

        // Turned away by integer rather than by ~ or %.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_integer_operators<double>);
}

// The concept is the exception specification, so the two must agree for every type.
BOOST_AUTO_TEST_CASE(NothrowIntegerOperatorsIsTheExceptionSpecification)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(1)) == xstd::nothrow_integer_operators<int>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(1, 1)) == xstd::nothrow_integer_operators<int>);

#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = xstd::test::absl_int128;
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(T{1})) == xstd::nothrow_integer_operators<T>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(T{1}, T{1})) == xstd::nothrow_integer_operators<T>);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
