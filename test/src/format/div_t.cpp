//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>         // int128_t
#include <xstd/cstdlib/div_t.hpp>   // div_t
#include <xstd/format/div_t.hpp>    // IWYU pragma: keep; formatter<div_t>
#include <xstd/format/int128.hpp>   // IWYU pragma: keep; formatter<int128_t>
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <cstdint>                  // exact-width integer types
#include <format>                   // format
#include <string_view>              // wstring_view
#include <tuple>                    // tuple

#ifdef __cpp_lib_constexpr_format
#include <xstd/test/constexpr.hpp> // XSTD_CONSTEXPR_CHECK_EQUAL
#endif

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t, xstd::int128_t>;

#ifdef __cpp_lib_format_ranges

// The formatting tests leave div_t's formatter and operator<< executed rather
// than merely instantiated - Boost.Test's printing machinery instantiates them
// for anything it might have to report, but only runs them when an assertion
// fails - and it is exactly the part an integer-class type need not support.
// The operator<< half lives in src/ostream.cpp.

// The formatting counterpart of XSTD_CONSTEXPR_CHECK_EQUAL, which cannot be
// used directly here: std::format is not a constant expression before P3391
// (constexpr std::format, plenary-approved for C++29), so the static_assert
// half only exists once a standard library announces the feature.
#ifdef __cpp_lib_constexpr_format
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) XSTD_CONSTEXPR_CHECK_EQUAL((a), (b))
#else
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) BOOST_CHECK_EQUAL((a), (b))
#endif

BOOST_AUTO_TEST_CASE_TEMPLATE(Formatter, T, exact_width_types)
{
        XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(std::format("{}", xstd::div_t<T>{1, -2}), "(1, -2)");
}

BOOST_AUTO_TEST_CASE(WideFormatter)
{
        BOOST_CHECK(std::format(L"{}", xstd::div_t<int>{1, -2}) == std::wstring_view{L"(1, -2)"});
}

#else

BOOST_AUTO_TEST_CASE(TupleFormatterUnavailable)
{
        BOOST_CHECK(true);
}

#endif
