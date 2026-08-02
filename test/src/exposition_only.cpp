//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/exposition_only.hpp>    // integral_class_type, is_integral_like
#include <xstd/test/constexpr.hpp>     // XSTD_CONSTEXPR_CHECK
#include <xstd/test/integer_class.hpp> // signed_integer_class, unsigned_integer_class
#include <boost/test/unit_test.hpp>    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                     // complex
#include <concepts>                    // integral, regular
#include <type_traits>                 // is_integral_v

BOOST_AUTO_TEST_SUITE(ExpositionOnly)

enum class color : unsigned { red = 1 };
struct not_a_number
{};

using S = xstd::test::signed_integer_class;
using U = xstd::test::unsigned_integer_class;

// The half of xstd::is_integral_like_v that std::is_integral cannot answer:
// the structural rendering of [iterator.concept.winc]'s integer-class types.
// Being a concept rather than a second variable template is what keeps the
// ill-formed spellings below *answers* rather than compile errors - a
// conjunction short-circuits during satisfaction checking, so
// std::numeric_limits is never instantiated over an array or a reference
// type.
BOOST_AUTO_TEST_CASE(IntegralClassType)
{
        // the shape it exists for: a class type with the arithmetic
        // operators, a std::numeric_limits specialization saying is_integer,
        // and explicit construction from int
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::integral_class_type<S>);
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::integral_class_type<U>);

        // the built-in integers are the *other* disjunct of the trait this
        // feeds, so they are not integer-class types here, exactly as they
        // are not in the standard's definition
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<char32_t>);

        // arithmetic without an integer's std::numeric_limits, and class
        // types with no std::numeric_limits at all
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<not_a_number>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<color>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int*>);

        // std::regular leads the rest of the conjunction so that these are
        // rejected before std::numeric_limits is instantiated over them: its
        // primary template declares a static member function returning I,
        // which for an array type is ill-formed rather than merely
        // unspecialized. Each of these four is a hard error rather than a
        // failing check if that ordering is disturbed.
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int()>);

        // and a cv-qualified class type, which is not assignable and so not
        // std::regular either
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<S const>);
}

// [iterator.concept.winc]'s is-integer-like: integral, or an integer-class
// type. The two disjuncts partition their domain, so every type here is
// answered by exactly one of them - which is what the checks against
// std::integral pin.
BOOST_AUTO_TEST_CASE(IsIntegralLike)
{
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integral_like<int> and std::integral<int>);
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integral_like<bool> and std::integral<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integral_like<S> and not std::integral<S>);
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integral_like<U> and not std::integral<U>);

        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<color>);

        // answered rather than hard-errored, which is the whole reason this
        // is a concept and xstd::is_integral_like_v reads it
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<int()>);

        // the one place this narrows what std::integral says yes to. const is
        // excluded by std::regular as well, volatile is not - which is why
        // the restriction is spelled with same_as and remove_cv_t rather than
        // left to the rest of the conjunction
        XSTD_CONSTEXPR_CHECK(std::integral<int const> and not xstd::exposition_only::is_integral_like<int const>);
        XSTD_CONSTEXPR_CHECK(std::integral<int volatile> and not xstd::exposition_only::is_integral_like<int volatile>);
        XSTD_CONSTEXPR_CHECK(std::regular<int volatile> and not std::regular<int const>);
}

// __int128 is the type the two disjuncts trade places over: libstdc++
// withholds it from std::is_integral outside GNU mode, where it is an
// integer-class type in everything but name, and reports it as integral
// inside GNU mode, where this concept declines it. Exactly one of the two
// holds either way, which is what keeps xstd::is_integral_like_v's answer
// dialect-independent.
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
BOOST_AUTO_TEST_CASE(Int128IsAnsweredByExactlyOneDisjunct)
{
        XSTD_CONSTEXPR_CHECK(std::is_integral_v<__int128> != xstd::exposition_only::integral_class_type<__int128>);
        XSTD_CONSTEXPR_CHECK(std::is_integral_v<unsigned __int128> != xstd::exposition_only::integral_class_type<unsigned __int128>);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif

BOOST_AUTO_TEST_SUITE_END()
