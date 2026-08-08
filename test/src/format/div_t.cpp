//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>                // int128
#include <xstd/cstdlib/div_t.hpp>          // div_t
#include <xstd/format/div_t.hpp>           // IWYU pragma: keep; formatter<div_t>
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <format>                          // format
#include <limits>                          // numeric_limits
#include <string_view>                     // wstring_view

#ifdef __cpp_lib_constexpr_format
#include <xstd/test/constexpr.hpp> // XSTD_CONSTEXPR_CHECK_EQUAL
#endif

BOOST_AUTO_TEST_SUITE(FormatDivT)

// Which of the formatter's two specializations runs is a property of the
// standard library and of S, so no expectation below names a path.

// XSTD_CONSTEXPR_CHECK_EQUAL for formatting, whose static_assert half exists
// only once a standard library announces P3391.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) XSTD_CONSTEXPR_CHECK_EQUAL((a), (b))
#else
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) BOOST_CHECK_EQUAL((a), (b))
#endif

BOOST_AUTO_TEST_CASE_TEMPLATE(Formatter, T, xstd::test::exact_width_signed_types)
{
        XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(std::format("{}", xstd::div_t<T>{1, -2}), "(1, -2)");
}

// Rendered by xstd where the standard library has no 128-bit formatter, so the
// widest value has to come out right rather than merely compile.
BOOST_AUTO_TEST_CASE(Boundaries)
{
        BOOST_CHECK_EQUAL(std::format("{}", xstd::div_t<xstd::int128>{std::numeric_limits<xstd::int128>::min(), 7}),
                          "(-170141183460469231731687303715884105728, 7)");
}

// Fill, alignment and width are the base's, and stay the base's: the formatter
// renders "(quot, rem)" and hands the whole of it over.
BOOST_AUTO_TEST_CASE(FillAndAlign)
{
        BOOST_CHECK_EQUAL(std::format("{:>12}", xstd::div_t<int>{1, -2}), "     (1, -2)");
        BOOST_CHECK_EQUAL(std::format("{:*^12}", xstd::div_t<int>{1, -2}), "**(1, -2)***");
}

BOOST_AUTO_TEST_CASE(WideFormatter)
{
        BOOST_CHECK(std::format(L"{}", xstd::div_t<int>{1, -2}) == std::wstring_view{L"(1, -2)"});
}

BOOST_AUTO_TEST_SUITE_END()
