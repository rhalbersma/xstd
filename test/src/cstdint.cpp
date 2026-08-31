//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>             // integer, signed_integer, unsigned_integer
#include <xstd/cstdint.hpp>              // XSTD_HAS_BIT_INT, bit_int, bit_uint, int128, uint128
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <xstd/type_traits.hpp>          // make_signed_t, make_unsigned_t
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                      // same_as
#include <limits>                        // numeric_limits

BOOST_AUTO_TEST_SUITE(Cstdint)

BOOST_AUTO_TEST_CASE(Int128)
{
        static_assert(sizeof(xstd::int128) == 16);
        static_assert(sizeof(xstd::uint128) == 16);
        static_assert(std::numeric_limits<xstd::int128>::is_signed);
        static_assert(not std::numeric_limits<xstd::uint128>::is_signed);
        static_assert(xstd::is_signed_v<xstd::int128 const>);
        static_assert(xstd::is_signed_v<xstd::int128 volatile>);
        static_assert(xstd::is_signed_v<xstd::int128 const volatile>);
        static_assert(xstd::is_unsigned_v<xstd::uint128 const>);
        static_assert(xstd::is_unsigned_v<xstd::uint128 volatile>);
        static_assert(xstd::is_unsigned_v<xstd::uint128 const volatile>);
        static_assert(std::numeric_limits<xstd::int128>::digits == 127);
        static_assert(std::numeric_limits<xstd::uint128>::digits == 128);
        static_assert(xstd::signed_integer<xstd::int128>);
        static_assert(xstd::unsigned_integer<xstd::uint128>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128>, xstd::int128>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128>, xstd::uint128>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 const>, xstd::int128 const>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 volatile>, xstd::int128 volatile>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 const volatile>, xstd::int128 const volatile>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 const>, xstd::uint128 const>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 volatile>, xstd::uint128 volatile>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 const volatile>, xstd::uint128 const volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::int128{-1} < xstd::int128{0});
        XSTD_CONSTEXPR_CHECK((xstd::uint128{1} << 127U) > xstd::uint128{0});
}

#ifdef XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_CASE(BitPreciseAliases)
{
        using S = xstd::bit_int<17>;
        using U = xstd::bit_uint<17>;

        static_assert(xstd::integer<S>);
        static_assert(xstd::integer<U>);
        static_assert(xstd::signed_integer<S>);
        static_assert(xstd::unsigned_integer<U>);
        static_assert(std::same_as<xstd::make_unsigned_t<S>, U>);
        static_assert(std::same_as<xstd::make_signed_t<U>, S>);
        static_assert(xstd::numeric_limits<S>::digits == 16);
        static_assert(xstd::numeric_limits<U>::digits == 17);
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::min() == S{-65536});
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::max() == S{65535});
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::max() == U{131071});

        // [iterator.concept.winc]/3's range, stated without naming the width.
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::min() + xstd::numeric_limits<S>::max() == -1);
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::min() == 0);
        XSTD_CONSTEXPR_CHECK((static_cast<U>(xstd::numeric_limits<S>::max()) * 2) + 1 == xstd::numeric_limits<U>::max());
}

#endif // XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_SUITE_END()
