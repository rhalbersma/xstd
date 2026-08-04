//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/floored_div.hpp> // floored_div
#include <xstd/cstdint.hpp>             // int128_t
#include <xstd/test/constexpr.hpp>      // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>     // Boost.Test
#include <cstdint>                      // exact-width integer types
#include <tuple>                        // tuple

BOOST_AUTO_TEST_SUITE(CStdLib)

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t, xstd::int128_t>;

BOOST_AUTO_TEST_CASE_TEMPLATE(FlooredDiv, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+8}, T{+3}) == xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+8}, T{-3}) == xstd::div_t<T>{-3, -1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-8}, T{+3}) == xstd::div_t<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-8}, T{-3}) == xstd::div_t<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+1}, T{+2}) == xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+1}, T{-2}) == xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-1}, T{+2}) == xstd::div_t<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-1}, T{-2}) == xstd::div_t<T>{0, -1}));
}

BOOST_AUTO_TEST_SUITE_END()
