//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/sign.hpp>    // sign
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp> // Boost.Test
#include <cstdint>                  // exact-width integer types
#include <limits>                   // numeric_limits
#include <tuple>                    // tuple

BOOST_AUTO_TEST_SUITE(CStdLib)

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Sign, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{-2}), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{-1}), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{0}), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{+1}), +1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{+2}), +1);

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::min()), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::max()), +1);
}

BOOST_AUTO_TEST_SUITE_END()
