//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                                // same_as

// Reached the way a consumer reaches it: the probe here, the adapter behind the umbrella.
#if __has_include(<boost/int128.hpp>)
#define TEST_HAS_BOOST_INT128
#include <xstd/ints/ext/boost.hpp> // the adapter for boost::int128's pair
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)

// One adapter behind this door, so no invariant spans two; that the associations arrive is what it answers for.
BOOST_AUTO_TEST_CASE(TheAssociationsArriveThroughTheUmbrella)
{
#ifdef TEST_HAS_BOOST_INT128
        using S = boost::int128::int128;
        using U = boost::int128::uint128;

        static_assert(std::same_as<xstd::make_unsigned_t<S>, U>);
        static_assert(std::same_as<xstd::make_signed_t<U>, S>);
        static_assert(xstd::signed_integer<S>);
        static_assert(xstd::unsigned_integer<U>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
