//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/bit_int.hpp>           // XSTD_HAS_BIT_INT, bit_int, bit_uint
#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                                // same_as

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdInt)
BOOST_AUTO_TEST_SUITE(BitInt)

#ifdef XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_CASE(AliasesModelTheIntegerConcepts)
{
        using S = xstd::bit_int<17>;
        using U = xstd::bit_uint<17>;

        static_assert(xstd::integer<S>);
        static_assert(xstd::integer<U>);
        static_assert(xstd::signed_integer<S>);
        static_assert(xstd::unsigned_integer<U>);
        static_assert(std::same_as<xstd::make_unsigned_t<S>, U>);
        static_assert(std::same_as<xstd::make_signed_t<U>, S>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(LimitsFollowTheRequestedWidth)
{
        using S = xstd::bit_int<17>;
        using U = xstd::bit_uint<17>;

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

#else

// The aliases are not declared without __BITINT_MAXWIDTH__, and Boost.Test wants a case either way.
BOOST_AUTO_TEST_CASE(AbsentWithoutCompilerSupport)
{
        BOOST_CHECK(true);
}

#endif // XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
