//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>        // signed_integral_like, unsigned_integral_like
#include <xstd/cstdint.hpp>         // int128_t, uint128_t
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK
#include <xstd/type_traits.hpp>     // make_signed_like_t, make_unsigned_like_t
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <limits>                   // numeric_limits
#include <type_traits>              // is_same_v

BOOST_AUTO_TEST_SUITE(Cstdint)

BOOST_AUTO_TEST_CASE(Int128)
{
        static_assert(sizeof(xstd::int128_t) == 16);
        static_assert(sizeof(xstd::uint128_t) == 16);
        static_assert(std::numeric_limits<xstd::int128_t>::is_signed);
        static_assert(not std::numeric_limits<xstd::uint128_t>::is_signed);
        static_assert(std::numeric_limits<xstd::int128_t>::digits == 127);
        static_assert(std::numeric_limits<xstd::uint128_t>::digits == 128);
        static_assert(xstd::signed_integral_like<xstd::int128_t>);
        static_assert(xstd::unsigned_integral_like<xstd::uint128_t>);
        static_assert(std::is_same_v<xstd::make_signed_like_t<xstd::uint128_t>, xstd::int128_t>);
        static_assert(std::is_same_v<xstd::make_unsigned_like_t<xstd::int128_t>, xstd::uint128_t>);
#ifdef _MSVC_STL_VERSION
        static_assert(std::is_same_v<xstd::int128_t, std::_Signed128>);
        static_assert(std::is_same_v<xstd::uint128_t, std::_Unsigned128>);
#endif

        XSTD_CONSTEXPR_CHECK(xstd::int128_t{-1} < xstd::int128_t{0});
        XSTD_CONSTEXPR_CHECK((xstd::uint128_t{1} << 127) > xstd::uint128_t{0});
}

BOOST_AUTO_TEST_SUITE_END()
