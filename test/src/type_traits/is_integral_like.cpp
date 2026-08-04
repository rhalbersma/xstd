//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/is_integral_like.hpp> // is_integral_like
#include <xstd/test/constexpr.hpp>               // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                               // complex
#include <type_traits>                           // false_type, is_integral_v, is_same_v, true_type

BOOST_AUTO_TEST_SUITE(TypeTraits)

enum class color : unsigned { red = 1 };

// The fourth opened trait, and the one whose standard counterpart answers
// from a closed list rather than from a property: std::is_integral_v is
// extended with [iterator.concept.winc]'s integer-class types, opened
// structurally in <xstd/concepts/exposition_only.hpp>. The cases below check
// the public trait spelling itself. Agreement with std::is_integral_v wherever
// it can answer is already covered by the sweep above, which this trait joins.
BOOST_AUTO_TEST_CASE(IsIntegralLike)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<char32_t>);

        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<color>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<std::complex<double>>);
}

// Total where the requirements it reaches are ill-formed rather than merely
// unsatisfied: they sit inside a concept, whose conjunction short-circuits, so
// each of these is an answer and not a compile error.
BOOST_AUTO_TEST_CASE(IsIntegralLikeIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<int()>);
}

// The C++23 category traits are cv-transparent, and so is the widening.
BOOST_AUTO_TEST_CASE(IsIntegralLikeIncludesCvQualifiedTypes)
{
        XSTD_CONSTEXPR_CHECK(std::is_integral_v<int const> and xstd::is_integral_like_v<int const>);
        XSTD_CONSTEXPR_CHECK(std::is_integral_v<int volatile> and xstd::is_integral_like_v<int volatile>);
}

// The bool_constant form, which is what std::conjunction and tag dispatch want
// and a concept cannot be.
BOOST_AUTO_TEST_CASE(IsIntegralLikeBoolConstant)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like<int>::value);
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::is_integral_like<int>, std::true_type>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::is_integral_like<double>, std::false_type>));
}

BOOST_AUTO_TEST_SUITE_END()
