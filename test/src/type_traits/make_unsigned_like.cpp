//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/make_unsigned_like.hpp> // make_unsigned_like
#include <xstd/test/constexpr.hpp>                 // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                 // complex
#include <cstdint>                                 // exact-width integer types
#include <type_traits>                             // is_same_v, make_unsigned_t

BOOST_AUTO_TEST_SUITE(TypeTraits)

enum class color : unsigned { red = 1 };

// A named concept rather than a bare requires-expression: an invalid operand
// that is also non-dependent is a hard error on GCC.
template<class T>
concept has_make_unsigned_like = requires { typename xstd::make_unsigned_like_t<T>; };

BOOST_AUTO_TEST_CASE(MakeUnsignedLike)
{
        // agrees with std::make_unsigned wherever std::make_unsigned answers,
        // for the unsigned types as much as the signed ones
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int8_t>, std::uint8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int16_t>, std::uint16_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int32_t>, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int64_t>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<int>, std::make_unsigned_t<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<unsigned>, unsigned>));

        // and answers "no" where std::make_unsigned is a hard error, which is
        // the whole point of an empty primary template
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<double>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<int*>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<color>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<void>);

        // cv-qualification is preserved, just as by std::make_unsigned.
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<int const>, unsigned const>));
}

BOOST_AUTO_TEST_SUITE_END()
