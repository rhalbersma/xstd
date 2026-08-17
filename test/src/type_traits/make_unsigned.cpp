//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned
#include <xstd/test/constexpr.hpp>            // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>           // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                            // complex
#include <cstdint>                            // exact-width integer types
#include <type_traits>                        // is_same_v, make_unsigned_t

BOOST_AUTO_TEST_SUITE(TypeTraits)

enum class color : unsigned { red = 1 };

// A named concept: a non-dependent invalid operand is a hard error on GCC.
template<class T>
concept has_make_unsigned = requires { typename xstd::make_unsigned_t<T>; };

BOOST_AUTO_TEST_CASE(MakeUnsignedLike)
{
        // agrees with std::make_unsigned wherever it answers, unsigned as much as signed
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<std::int8_t>, std::uint8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<std::int16_t>, std::uint16_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<std::int32_t>, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<std::int64_t>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<int>, std::make_unsigned_t<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<unsigned>, unsigned>));

        // and answers no where std::make_unsigned hard-errors: the point of an empty primary
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<double>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<int*>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<color>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<void>);

        // cv-qualification is preserved, just as by std::make_unsigned.
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_t<int const>, unsigned const>));
}

BOOST_AUTO_TEST_SUITE_END()
