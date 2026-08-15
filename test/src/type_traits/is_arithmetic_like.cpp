//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/is_arithmetic_like.hpp> // is_arithmetic_like
#include <xstd/type_traits/is_integral_like.hpp>   // is_integral_like_v
#include <xstd/test/constexpr.hpp>                 // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <limits>                                  // numeric_limits
// A class that says it is a number but not an integer; only its numeric_limits matters.

// clang-format off
struct not_an_integer_class_type {};

template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
struct std::numeric_limits<not_an_integer_class_type> : std::numeric_limits<double> {};
// clang-format on

BOOST_AUTO_TEST_SUITE(TypeTraits)

// The half xstd has not opened: only the integral disjunct is widened.
BOOST_AUTO_TEST_CASE(ArithmeticLikeOpensTheIntegralHalfOnly)
{
        XSTD_CONSTEXPR_CHECK(std::numeric_limits<not_an_integer_class_type>::is_specialized);
        XSTD_CONSTEXPR_CHECK(not std::numeric_limits<not_an_integer_class_type>::is_integer);

        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<not_an_integer_class_type>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_arithmetic_like_v<not_an_integer_class_type>);

        // the built-in floating-point types are reported exactly as the standard does
        XSTD_CONSTEXPR_CHECK(xstd::is_arithmetic_like_v<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<double>);
}

BOOST_AUTO_TEST_SUITE_END()
