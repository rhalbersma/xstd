//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/exposition_only.hpp> // integer_class_type, is_integer_like
#include <xstd/test/constexpr.hpp>           // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>          // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <complex>                           // complex
#include <concepts>                          // integral, regular
#include <limits>                            // numeric_limits
#include <type_traits>                       // is_integral_v

BOOST_AUTO_TEST_SUITE(ExpositionOnly)

enum class color : unsigned { red = 1 };
// clang-format off
struct not_a_number {};
// clang-format on

// The half of xstd::is_integral_like_v that std::is_integral cannot answer:
// the structural rendering of [iterator.concept.winc]'s integer-class types.
// Being a concept rather than a second variable template is what keeps the
// ill-formed spellings below *answers* rather than compile errors - a
// conjunction short-circuits during satisfaction checking, so
// std::numeric_limits is never instantiated over an array or a reference
// type.
BOOST_AUTO_TEST_CASE(IntegerClassType)
{
        // the built-in integers are the *other* disjunct of the trait this
        // feeds, so they are not integer-class types here, exactly as they
        // are not in the standard's definition
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<char32_t>);

        // arithmetic without an integer's std::numeric_limits, and class
        // types with no std::numeric_limits at all
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<not_a_number>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<color>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<int*>);
}

// The completeness guard and expression requirements precede numeric_limits:
// its primary template declares a static member function returning I, which
// for an array type is ill-formed rather than merely unspecialized. Each of
// these is a hard error rather than a failing check if that ordering is
// disturbed.
BOOST_AUTO_TEST_CASE(IntegerClassTypeIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<int()>);

        // and an incomplete class type, which the sizeof term answers before
        // std::regular is asked a question that needs a complete type
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integer_class_type<struct never_defined>);
}

// [iterator.concept.winc]'s is-integer-like: integral, or an integer-class
// type. The two disjuncts partition their domain, so every type here is
// answered by exactly one of them - which is what the checks against
// std::integral pin.
BOOST_AUTO_TEST_CASE(IsIntegerLike)
{
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integer_like<int> and std::integral<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<bool> and std::integral<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<color>);
}

// Answered rather than hard-errored, which is the whole reason this is a
// concept and xstd::is_integral_like_v reads it rather than spelling the
// requirements out as its own initializer.
BOOST_AUTO_TEST_CASE(IsIntegerLikeIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<struct also_never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integer_like<int()>);
}

// The built-in branch follows std::integral's cv-transparent trait.
BOOST_AUTO_TEST_CASE(IsIntegerLikeIncludesCvQualifiedTypes)
{
        XSTD_CONSTEXPR_CHECK(std::integral<int const> and xstd::exposition_only::is_integer_like<int const>);
        XSTD_CONSTEXPR_CHECK(std::integral<int volatile> and xstd::exposition_only::is_integer_like<int volatile>);
}

BOOST_AUTO_TEST_SUITE_END()
