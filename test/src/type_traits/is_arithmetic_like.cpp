//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/is_arithmetic_like.hpp> // is_arithmetic_like
#include <xstd/test/constexpr.hpp>                 // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <limits>                                  // numeric_limits
// A class type that says it is a number but not an integer - the shape a
// future floating-point-class opening would cover. Only what its
// std::numeric_limits reports matters, so it needs no members and no
// operators: the specialization is the whole fixture. Both live out here
// because an explicit specialization of a standard-library template has to be
// at global scope, and BOOST_AUTO_TEST_SUITE opens a namespace.
// clang-format off
struct not_an_integer_class_type {};

template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
struct std::numeric_limits<not_an_integer_class_type> : std::numeric_limits<double> {};
// clang-format on

BOOST_AUTO_TEST_SUITE(TypeTraits)

// The half xstd has *not* opened. is_arithmetic_like_v is spelled the way the
// standard spells is_arithmetic_v - an integral type, or a floating-point one
// - with only the integral half replaced by an open one, so a class type that
// behaves like a floating-point number is not arithmetic-like today. It is the
// obvious next thing to open, and the shape of the trait is what leaves room
// for it: a second exposition-only concept, a second disjunct, nothing else
// moved.
BOOST_AUTO_TEST_CASE(ArithmeticLikeOpensTheIntegralHalfOnly)
{
        XSTD_CONSTEXPR_CHECK(std::numeric_limits<not_an_integer_class_type>::is_specialized);
        XSTD_CONSTEXPR_CHECK(not std::numeric_limits<not_an_integer_class_type>::is_integer);

        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<not_an_integer_class_type>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_arithmetic_like_v<not_an_integer_class_type>);

        // the built-in floating-point types are the standard's half, reported
        // exactly as the standard reports them
        XSTD_CONSTEXPR_CHECK(xstd::is_arithmetic_like_v<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<double>);
}

BOOST_AUTO_TEST_SUITE_END()
