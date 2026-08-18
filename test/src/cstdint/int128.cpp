//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint/int128.hpp>            // int128, uint128
#include <xstd/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <boost/test/unit_test.hpp>           // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                           // same_as

BOOST_AUTO_TEST_SUITE(CstdintInt128)

BOOST_AUTO_TEST_CASE(AssociatesSignedAndUnsignedPair)
{
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128>, xstd::int128>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128>, xstd::uint128>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 const>, xstd::int128 const>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 const>, xstd::uint128 const>);
}

BOOST_AUTO_TEST_SUITE_END()
