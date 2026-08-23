//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer.hpp>                 // NOLINT(misc-include-cleaner): used with optional Abseil fixtures
#include <xstd/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/cstdint.hpp>                          // int128, uint128
#include <xstd/cstdlib/abs.hpp>                      // abs
#include <xstd/cstdlib/div.hpp>                      // div
#include <xstd/test/constexpr_check.hpp>             // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                  // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                   // complex

// Reached the way a consumer reaches it: the probe here, the pair inside the adapter.
#if __has_include(<absl/numeric/int128.h>)
#define XSTD_TEST_HAS_ABSL_INT128
#include <xstd/ext/absl/int128.hpp> // int128, uint128
#endif

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(NothrowConstOperators)
{
        // The built-in widths and the 128-bit type the library names all carry it.
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<char32_t>);

        // Its first conjunct is integer, which cv bool is not, so the operators go unasked.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128> and xstd::nothrow_const_operators<xstd::uint128>);

#ifdef XSTD_HAS_BIT_INT
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::bit_int<17>>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::bit_uint<17>>);
#endif

        // Admitted by integer and refused here: why the two are separate concepts.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        XSTD_CONSTEXPR_CHECK(xstd::integer<absl::int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<absl::int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<absl::uint128>);
#endif
}

// Cv-transparent on both branches; volatile is the one that takes the stripping.
BOOST_AUTO_TEST_CASE(NothrowConstOperatorsIsCvTransparentOnBothBranches)
{
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int const volatile>);

        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128 const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128 volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128 const volatile>);

        // Nor does a qualifier turn a no into a yes.
#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = absl::int128;

        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T const>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T volatile>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T const volatile>);
#endif
}

// Total: the conjunction short-circuits on integer before any operator is named.
BOOST_AUTO_TEST_CASE(NothrowConstOperatorsIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<void>);
        // remove_cv_t, not remove_cvref_t: a reference is not an integer type.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int()>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<std::complex<double>>);

        // Turned away by integer rather than by ~ or %.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<double>);
}

// The concept is the exception specification, so the two must agree for every type.
BOOST_AUTO_TEST_CASE(NothrowConstOperatorsIsTheExceptionSpecification)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(1)) == xstd::nothrow_const_operators<int>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(1, 1)) == xstd::nothrow_const_operators<int>);

#ifdef XSTD_TEST_HAS_ABSL_INT128
        using T = absl::int128;
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(T{1})) == xstd::nothrow_const_operators<T>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(T{1}, T{1})) == xstd::nothrow_const_operators<T>);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
