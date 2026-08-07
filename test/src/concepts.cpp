//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>        // specialization_of, integral_like, signed_integral_like, unsigned_integral_like
#include <xstd/type_traits.hpp>     // empty_type, is_integral_like_v, is_specialization_of_v
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK
#include <complex>                  // complex
#include <concepts>                 // signed_integral, unsigned_integral
#include <cstdint>                  // int8_t, int16_t, int32_t, int64_t
#include <tuple>                    // tuple
#include <type_traits>              // make_unsigned_t

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(SpecializationOfAgreesWithItsTrait)
{
        // nothing in the concept is specific to the standard library. The
        // library's own class template is a program-defined primary like any
        // other, with its tag declared in place
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<xstd::empty_type<struct user_tag>, xstd::empty_type>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<int, xstd::empty_type>));

        // the concept agrees with the trait it is spelled over
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::complex<int>, std::complex> == xstd::is_specialization_of_v<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<int, std::complex> == xstd::is_specialization_of_v<int, std::complex>));
}

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

// The property that makes this a widening of <concepts> rather than a
// replacement for it: whatever std::integral accepts, integral_like accepts,
// and with the same signedness. Checked per width rather than once, since the
// signed and unsigned answers have to line up at each.
BOOST_AUTO_TEST_CASE_TEMPLATE(IntegralLikeIsASupersetOfIntegral, T, exact_width_types)
{
        using U = std::make_unsigned_t<T>;

        static_assert(std::signed_integral<T> and xstd::signed_integral_like<T>);
        static_assert(std::unsigned_integral<U> and xstd::unsigned_integral_like<U>);

        static_assert(xstd::integral_like<T> and xstd::integral_like<U>);
        static_assert(not xstd::unsigned_integral_like<T>);
        static_assert(not xstd::signed_integral_like<U>);

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

// The concept agrees with the trait it is spelled over, on the types where
// that spelling is what keeps the question answerable at all: the trait's own
// answers, and the reasons they are answers rather than compile errors, are
// pinned in src/type_traits.cpp, where it is defined.
BOOST_AUTO_TEST_CASE(IntegralLikeAgreesWithItsTrait)
{
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int> == xstd::is_integral_like_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<double> == xstd::is_integral_like_v<double>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<void> == xstd::is_integral_like_v<void>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int[3]> == xstd::is_integral_like_v<int[3]>);
}

// Overloading on integral_like and signed_integral_like has to partial-order
// the way std::integral and std::signed_integral do. It only does so because
// each of the narrower concepts is spelled "integral_like<T> and ...", so its
// normal form contains integral_like's; a formulation that repeated the
// requirements, or that gave either of the narrower concepts a flat trait of
// its own to be spelled over, would make this call ambiguous rather than
// picking the second overload.
template<xstd::integral_like I>
[[nodiscard]] constexpr auto which(I) noexcept -> int
{
        return 1;
}

template<xstd::signed_integral_like S>
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
