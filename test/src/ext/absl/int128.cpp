//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>             // signed_integer, unsigned_integer
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <xstd/type_traits.hpp>          // make_signed, make_unsigned
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <concepts>                      // same_as

// Reached the way a consumer reaches it: the probe here, the pair inside the adapter.
#if __has_include(<absl/numeric/int128.h>)
#define XSTD_TEST_HAS_ABSL_INT128
#include <xstd/ext/absl/int128.hpp> // int128, uint128
#endif

BOOST_AUTO_TEST_SUITE(ExtAbslInt128)

BOOST_AUTO_TEST_CASE(AssociationsPairTheTwoHalves)
{
#ifdef XSTD_TEST_HAS_ABSL_INT128
        using S = absl::int128;
        using U = absl::uint128;

        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<S>, U>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<U>, S>));

        // Each half is its own, which is how integer tells a pair from a third type beside one.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<S>, S>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<U>, U>));

        // Each half reached from the other and back, and the cv qualifiers carried along.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<xstd::make_unsigned_t<S>>, S>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<xstd::make_signed_t<U>>, U>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<U const>, S const>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<S volatile>, U volatile>));
#endif
        BOOST_CHECK(true);
}

// Which is the whole of what integer asks beyond integer_class, and what the shipped header supplies.
BOOST_AUTO_TEST_CASE(WhichIsWhatCarriesThePairIntoTheConcepts)
{
#ifdef XSTD_TEST_HAS_ABSL_INT128
        XSTD_CONSTEXPR_CHECK(xstd::signed_integer<absl::int128>);
        XSTD_CONSTEXPR_CHECK(xstd::unsigned_integer<absl::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
