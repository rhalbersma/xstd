//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integral_like.hpp>      // integral_like
#include <xstd/concepts/nothrow_arithmetic.hpp> // nothrow_arithmetic
#include <xstd/cstdint.hpp>                     // int128, uint128
#include <xstd/cstdlib/abs.hpp>                 // abs
#include <xstd/cstdlib/div.hpp>                 // div
#include <xstd/test/constexpr.hpp>              // XSTD_CONSTEXPR_CHECK
#include <xstd/test/unannotated.hpp>            // unannotated
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                              // complex

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(NothrowArithmetic)
{
        // Every built-in width, and the 128-bit type the library names, come
        // with the specifier - on GCC and Clang because __int128's operators
        // are built in, on MSVC because std::_Signed128 writes it.
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_arithmetic<int>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_arithmetic<bool> and xstd::nothrow_arithmetic<char32_t>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_arithmetic<xstd::int128> and xstd::nothrow_arithmetic<xstd::uint128>);

        // An integer-class type whose author wrote no specifier is admitted by
        // integral_like and refused here. That is the whole point of the two
        // being separate concepts: [iterator.concept.winc] asks for none of
        // this, so requiring it of the type would narrow the extension point,
        // where requiring it of the exception specification narrows nothing.
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<xstd::test::unannotated>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<xstd::test::unannotated>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<xstd::test::unannotated_unsigned>);
}

// Total on the spellings that would be ill-formed if the requirements were
// reached, for the same reason integral_like is: they sit inside a concept,
// whose conjunction short-circuits on integral_like before any operator is
// named. A caller can ask this of any type and get an answer.
BOOST_AUTO_TEST_CASE(NothrowArithmeticIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<int()>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<std::complex<double>>);

        // Floating point is turned away by integral_like rather than by any
        // operator, which is what keeps ~, % and the bitwise requirements from
        // ever being asked of a type that has none of them.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_arithmetic<double>);
}

// The concept is the exception specification, not a description of one: the
// six functions are declared noexcept(nothrow_arithmetic<S>), so these two
// have to agree for every element type, and a type that flips one flips both.
BOOST_AUTO_TEST_CASE(NothrowArithmeticIsTheExceptionSpecification)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(1)) == xstd::nothrow_arithmetic<int>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(1, 1)) == xstd::nothrow_arithmetic<int>);

        using T = xstd::test::unannotated;
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(T{1})) == xstd::nothrow_arithmetic<T>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(T{1}, T{1})) == xstd::nothrow_arithmetic<T>);
}

BOOST_AUTO_TEST_SUITE_END()
