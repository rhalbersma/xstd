//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integral_like.hpp> // integral_like
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE

BOOST_AUTO_TEST_SUITE(Concepts)

enum class scoped : unsigned { s0 };

BOOST_AUTO_TEST_CASE(IntegralLike)
{
        // The built-in branch follows std::integral, including its treatment
        // of bool as an unsigned integral type.
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<char> and xstd::integral_like<char32_t>);

        // nothing whose std::numeric_limits says it is not an integer
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<scoped>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int*>);

        // the spellings that are answered rather than hard-errored only
        // because the requirements they would trip over sit inside a concept,
        // where a conjunction short-circuits. These four pin that behavior in
        // the public concept a caller actually writes.
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int()>);

        // C++23's arithmetic concepts inherit the category traits' cv
        // transparency, and the widened concepts do the same.
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int const>);
}

BOOST_AUTO_TEST_SUITE_END()
