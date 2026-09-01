//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_down.hpp> // align_down
#include <xstd/test/constexpr_check.hpp>   // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                        // same_as
#include <cstddef>                         // size_t
#include <cstdint>                         // uint8_t, uint16_t
#include <limits>                          // numeric_limits

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Memory)
BOOST_AUTO_TEST_SUITE(AlignDown)

BOOST_AUTO_TEST_CASE(RoundsDownToThePrecedingMultiple)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down( 0UZ, 8),  0UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down( 1UZ, 8),  0UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down( 8UZ, 8),  8UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down( 9UZ, 8),  8UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down(64UZ, 8), 64UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down(65UZ, 8), 64UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down(std::numeric_limits<std::size_t>::max(), 8), std::numeric_limits<std::size_t>::max() - 7);
        // clang-format on
}

// The narrow types stay themselves, where the operands alone would promote.
BOOST_AUTO_TEST_CASE(KeepsTheNarrowOperandType)
{
        static_assert(std::same_as<decltype(xstd::align_down(std::uint8_t{}, 4)), std::uint8_t>);
        static_assert(std::same_as<decltype(xstd::align_down(std::uint16_t{}, 4)), std::uint16_t>);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down(std::uint8_t{5}, 4), std::uint8_t{4});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
