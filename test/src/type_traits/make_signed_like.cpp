//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/make_signed_like.hpp> // make_signed_like
#include <xstd/test/constexpr.hpp>               // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <cstdint>                               // int8_t, uint8_t
#include <type_traits>                           // is_same_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

// A named concept rather than a bare requires-expression in the test body: a
// requires-expression whose operand is invalid *and* non-dependent is a hard
// error on GCC, so the type has to stay a template parameter.
template<class T>
concept has_make_signed_like = requires { typename xstd::make_signed_like_t<T>; };

BOOST_AUTO_TEST_CASE(MakeSignedLike)
{
        // The signed half is the exact mirror of the unsigned half.
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<std::uint8_t>, std::int8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<unsigned>, int>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<unsigned const>, int const>));
        XSTD_CONSTEXPR_CHECK(not has_make_signed_like<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_signed_like<double>);
}

BOOST_AUTO_TEST_SUITE_END()
