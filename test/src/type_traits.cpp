//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>     // opened numeric traits and sign modifiers
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                  // complex
#include <type_traits>              // standard traits

BOOST_AUTO_TEST_SUITE(TypeTraits)

enum class color : unsigned { red = 1 };

// An incomplete class type is answered, not hard-errored: the traits being
// widened cope with one, so these have to as well. It is the sizeof term in
// xstd::exposition_only::integer_class_type that keeps them total - every
// question after it, std::regular's std::destructible first, needs a complete
// type to be well-formed rather than merely false.
BOOST_AUTO_TEST_CASE(OpenedNumericTraitsAnswerForIncompleteTypes)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<struct never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_arithmetic_like_v<struct never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_signed_like_v<struct never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_unsigned_like_v<struct never_defined>);
}

// The four opened numeric traits. What makes them widenings rather than
// replacements is that they agree with the standard's on every type the
// standard's can answer for, so these are checked against std::is_arithmetic_v
// / std::is_signed_v / std::is_unsigned_v / std::is_integral_v directly rather
// than against hand-written expectations - a table of expected answers could
// drift, an equality against the trait being widened cannot.
template<class T>
auto check_agrees_with_std()
        -> void
{
        XSTD_CONSTEXPR_CHECK(xstd::is_arithmetic_like_v<T> == std::is_arithmetic_v<T>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<T> == std::is_signed_v<T>);
        XSTD_CONSTEXPR_CHECK(xstd::is_unsigned_like_v<T> == std::is_unsigned_v<T>);
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<T> == std::is_integral_v<T>);
}

BOOST_AUTO_TEST_CASE(OpenedNumericTraitsAgreeWithStd)
{
        check_agrees_with_std<char>();
        check_agrees_with_std<signed char>();
        check_agrees_with_std<unsigned char>();
        check_agrees_with_std<short>();
        check_agrees_with_std<int>();
        check_agrees_with_std<unsigned>();
        check_agrees_with_std<long long>();
        check_agrees_with_std<float>();
        check_agrees_with_std<double>();

        // and on the types neither can call arithmetic
        check_agrees_with_std<int*>();
        check_agrees_with_std<color>();
        check_agrees_with_std<std::complex<double>>();
        check_agrees_with_std<void>();
        check_agrees_with_std<int&>();
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        check_agrees_with_std<int[3]>();
}

// Sign modifiers are inverse transformations for supported integral-like types.
BOOST_AUTO_TEST_CASE(MakeSignedAndUnsignedLikeAreInverses)
{
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<xstd::make_unsigned_like_t<int>>, int>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<xstd::make_signed_like_t<unsigned>>, unsigned>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<xstd::make_unsigned_like_t<int const>>, int const>));
}

BOOST_AUTO_TEST_SUITE_END()
