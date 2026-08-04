//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>         // int128_t, uint128_t
#include <xstd/format/int128.hpp>   // detail::to_decimal, formatter<int128_t>
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL
#include <format>                   // format
#include <limits>                   // numeric_limits

BOOST_AUTO_TEST_CASE(Int128Decimal)
{
        BOOST_CHECK_EQUAL(xstd::detail::to_decimal(xstd::int128_t{0}), "0");
        BOOST_CHECK_EQUAL(xstd::detail::to_decimal(xstd::int128_t{-1}), "-1");
        BOOST_CHECK_EQUAL(xstd::detail::to_decimal(std::numeric_limits<xstd::int128_t>::min()), "-170141183460469231731687303715884105728");
        BOOST_CHECK_EQUAL(xstd::detail::to_decimal(std::numeric_limits<xstd::int128_t>::max()), "170141183460469231731687303715884105727");
        BOOST_CHECK_EQUAL(xstd::detail::to_decimal(std::numeric_limits<xstd::uint128_t>::max()), "340282366920938463463374607431768211455");
}

BOOST_AUTO_TEST_CASE(Int128Formatter)
{
        BOOST_CHECK_EQUAL(std::format("{}", xstd::int128_t{0}), "0");
        BOOST_CHECK_EQUAL(std::format("{}", xstd::int128_t{-1}), "-1");
        BOOST_CHECK_EQUAL(std::format("{}", std::numeric_limits<xstd::int128_t>::min()), "-170141183460469231731687303715884105728");
        BOOST_CHECK_EQUAL(std::format("{}", std::numeric_limits<xstd::int128_t>::max()), "170141183460469231731687303715884105727");
        BOOST_CHECK_EQUAL(std::format("{}", std::numeric_limits<xstd::uint128_t>::max()), "340282366920938463463374607431768211455");
}
