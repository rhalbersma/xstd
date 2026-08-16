//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>               // specialization_of, integer_like, signed_integer_like, unsigned_integer_like
#include <xstd/type_traits.hpp>            // empty_type, is_integer_like_v, is_specialization_of_v
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK
#include <xstd/test/exact_width_types.hpp> // std_signed_types
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK
#include <complex>                         // complex
#include <concepts>                        // signed_integral, unsigned_integral
#include <type_traits>                     // make_unsigned_t

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(SpecializationOfAgreesWithItsTrait)
{
        // nothing in the concept is specific to the standard library
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<xstd::empty_type<struct user_tag>, xstd::empty_type>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<int, xstd::empty_type>));

        // the concept agrees with the trait it is spelled over
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::complex<int>, std::complex> == xstd::is_specialization_of_v<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<int, std::complex> == xstd::is_specialization_of_v<int, std::complex>));
}

// A widening, not a replacement: whatever std::integral accepts, this accepts.
BOOST_AUTO_TEST_CASE_TEMPLATE(IntegralLikeIsASupersetOfIntegral, T, xstd::test::std_signed_types)
{
        using U = std::make_unsigned_t<T>;

        static_assert(std::signed_integral<T> and xstd::signed_integer_like<T>);
        static_assert(std::unsigned_integral<U> and xstd::unsigned_integer_like<U>);

        static_assert(xstd::integer_like<T> and xstd::integer_like<U>);
        static_assert(not xstd::unsigned_integer_like<T>);
        static_assert(not xstd::signed_integer_like<U>);

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// The concept agrees with the trait it is spelled over.
BOOST_AUTO_TEST_CASE(IntegralLikeAgreesWithItsTrait)
{
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int> == xstd::is_integer_like_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<double> == xstd::is_integer_like_v<double>);
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<void> == xstd::is_integer_like_v<void>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(xstd::integer_like<int[3]> == xstd::is_integer_like_v<int[3]>);
}

// Partial-orders as the standard's do, because each narrower one keeps integer_like atomic.
template<xstd::integer_like I>
[[nodiscard]] constexpr auto which(I) noexcept -> int
{
        return 1;
}

template<xstd::signed_integer_like S>
[[nodiscard]] constexpr auto which(S) noexcept -> int
{
        return 2;
}

BOOST_AUTO_TEST_CASE(NarrowerConceptsSubsume)
{
        XSTD_CONSTEXPR_CHECK(which(1) == 2);
        XSTD_CONSTEXPR_CHECK(which(1U) == 1);
}

BOOST_AUTO_TEST_SUITE_END()
