//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>         // int128_t, uint128_t
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <limits>                   // numeric_limits

BOOST_AUTO_TEST_SUITE(Cstdint)

BOOST_AUTO_TEST_CASE(Int128)
{
        static_assert(sizeof(xstd::int128_t) == 16);
        static_assert(sizeof(xstd::uint128_t) == 16);
        static_assert(std::numeric_limits<xstd::int128_t>::is_signed);
        static_assert(not std::numeric_limits<xstd::uint128_t>::is_signed);
        static_assert(std::numeric_limits<xstd::int128_t>::digits == 127);
        static_assert(std::numeric_limits<xstd::uint128_t>::digits == 128);

        XSTD_CONSTEXPR_CHECK(xstd::int128_t{-1} < xstd::int128_t{0});
        XSTD_CONSTEXPR_CHECK((xstd::uint128_t{1} << 127) > xstd::uint128_t{0});
}

BOOST_AUTO_TEST_SUITE_END()
