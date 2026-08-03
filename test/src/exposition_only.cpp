//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/exposition_only/integral-class.hpp> // integral_class_type, is_integral_like
#include <xstd/test/constexpr.hpp>                          // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <complex>                                          // complex
#include <concepts>                                         // integral, regular
#include <limits>                                           // numeric_limits
#include <type_traits>                                      // is_integral_v

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
BOOST_AUTO_TEST_CASE(IntegralClassType)
{
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
}

// std::regular leads the rest of the conjunction so that these are rejected
// before std::numeric_limits is instantiated over them: its primary template
// declares a static member function returning I, which for an array type is
// ill-formed rather than merely unspecialized. Each of these four is a hard
// error rather than a failing check if that ordering is disturbed.
BOOST_AUTO_TEST_CASE(IntegralClassTypeIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<int()>);

        // and an incomplete class type, which the sizeof term answers before
        // std::regular is asked a question that needs a complete type
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::integral_class_type<struct never_defined>);
}

// [iterator.concept.winc]'s is-integer-like: integral, or an integer-class
// type. The two disjuncts partition their domain, so every type here is
// answered by exactly one of them - which is what the checks against
// std::integral pin.
BOOST_AUTO_TEST_CASE(IsIntegralLike)
{
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integral_like<int> and std::integral<int>);
        XSTD_CONSTEXPR_CHECK(xstd::exposition_only::is_integral_like<bool> and std::integral<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<color>);
}

// Answered rather than hard-errored, which is the whole reason this is a
// concept and xstd::is_integral_like_v reads it rather than spelling the
// requirements out as its own initializer.
BOOST_AUTO_TEST_CASE(IsIntegralLikeIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<struct also_never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::exposition_only::is_integral_like<int()>);
}

// The built-in branch follows std::integral's cv-transparent trait.
BOOST_AUTO_TEST_CASE(IsIntegralLikeIncludesCvQualifiedTypes)
{
        XSTD_CONSTEXPR_CHECK(std::integral<int const> and xstd::exposition_only::is_integral_like<int const>);
        XSTD_CONSTEXPR_CHECK(std::integral<int volatile> and xstd::exposition_only::is_integral_like<int volatile>);
}

// __int128 is the type the two disjuncts trade places over: libstdc++
// withholds it from std::is_integral outside GNU mode, where it is an
// integer-class type in everything but name, and reports it as integral
// inside GNU mode, where integral_class_type declines it. Exactly one of the
// two claims it either way, which is what keeps is_integral_like's answer
// dialect-independent. Naming the type is what -Wpedantic is for, hence the
// same suppression the headers carry.
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

// __SIZEOF_INT128__ says the *compiler* has the type. It says nothing about
// whether the *standard library* describes it, and those come apart: clang-cl
// has __int128 on x64, but the MSVC STL specializes neither std::is_integral
// nor std::numeric_limits for it, so neither disjunct claims it and
// is_integral_like correctly declines. std::numeric_limits is the gate
// because it is what integral_class_type actually reads - the same gate, for
// the same reason, as test/src/cstdlib.cpp's.
//
// A template so that the discarded branch is never instantiated: in a
// non-templated context a discarded if-constexpr statement is still fully
// checked, static_asserts included.
template<class T>
auto check_int128()
        -> void
{
        if constexpr (std::numeric_limits<T>::is_specialized) {
                static_assert(std::is_integral_v<T> != xstd::exposition_only::integral_class_type<T>);
                static_assert(xstd::exposition_only::is_integral_like<T>);
        }
}

BOOST_AUTO_TEST_CASE(Int128IsClaimedByExactlyOneDisjunct)
{
        check_int128<__int128>();
        check_int128<unsigned __int128>();

        BOOST_CHECK(true); // the MSVC STL leaves __int128 undescribed, see above
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif

BOOST_AUTO_TEST_SUITE_END()
