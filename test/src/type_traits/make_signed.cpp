//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/make_signed.hpp> // make_signed
#include <xstd/test/constexpr.hpp>          // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <cstdint>                          // int8_t, uint8_t
#include <type_traits>                      // is_same_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

enum class color : unsigned { red = 1 };

// A named concept: a non-dependent invalid operand is a hard error on GCC.
template<class T>
concept has_make_signed = requires { typename xstd::make_signed_t<T>; };

BOOST_AUTO_TEST_CASE(MakeSignedLike)
{
        // The signed half is the exact mirror of the unsigned half.
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_t<std::uint8_t>, std::int8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_t<unsigned>, int>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_t<unsigned const>, int const>));
        XSTD_CONSTEXPR_CHECK(not has_make_signed<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_signed<double>);
        // An enumeration std::make_signed would answer for, this library not being asked it.
        XSTD_CONSTEXPR_CHECK(not has_make_signed<color>);
}

BOOST_AUTO_TEST_SUITE_END()
