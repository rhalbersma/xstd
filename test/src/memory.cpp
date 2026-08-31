//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/memory.hpp>               // align_down, align_up
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/align/align_down.hpp>    // align_down
#include <boost/align/align_up.hpp>      // align_up
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                      // same_as
#include <cstddef>                       // size_t
#include <cstdint>                       // uint8_t, uint16_t, uint32_t, uint64_t
#include <limits>                        // numeric_limits

BOOST_AUTO_TEST_SUITE(Memory)

BOOST_AUTO_TEST_CASE(AlignUp)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 0UZ, 8),  0UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 1UZ, 8),  8UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 8UZ, 8),  8UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 9UZ, 8), 16UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(64UZ, 8), 64UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(65UZ, 8), 72UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(std::numeric_limits<std::size_t>::max() - 7, 8), std::numeric_limits<std::size_t>::max() - 7);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(std::numeric_limits<std::size_t>::max() - 8, 8), std::numeric_limits<std::size_t>::max() - 7);
        // clang-format on
}

BOOST_AUTO_TEST_CASE(AlignDown)
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
BOOST_AUTO_TEST_CASE(ReturnType)
{
        static_assert(std::same_as<decltype(xstd::align_up(std::uint8_t{}, 4)), std::uint8_t>);
        static_assert(std::same_as<decltype(xstd::align_down(std::uint8_t{}, 4)), std::uint8_t>);
        static_assert(std::same_as<decltype(xstd::align_up(std::uint16_t{}, 4)), std::uint16_t>);
        static_assert(std::same_as<decltype(xstd::align_down(std::uint16_t{}, 4)), std::uint16_t>);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(std::uint8_t{5}, 4), std::uint8_t{8});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_down(std::uint8_t{5}, 4), std::uint8_t{4});
}

BOOST_AUTO_TEST_CASE(BoostAlign)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK(xstd::align_up  ( 0UZ, 8) == boost::alignment::align_up  ( 0UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_up  ( 1UZ, 8) == boost::alignment::align_up  ( 1UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_up  ( 9UZ, 8) == boost::alignment::align_up  ( 9UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_up  (65UZ, 8) == boost::alignment::align_up  (65UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down( 0UZ, 8) == boost::alignment::align_down( 0UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down( 1UZ, 8) == boost::alignment::align_down( 1UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down( 9UZ, 8) == boost::alignment::align_down( 9UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down(65UZ, 8) == boost::alignment::align_down(65UZ, 8));
        // clang-format on
}

// Every value and power-of-two alignment below the width where a result still fits.
template<class I>
auto agrees_with_boost_align() -> void
{
        for (auto alignment = 1UZ; alignment <= 16UZ; alignment *= 2UZ) {
                for (auto value = 0UZ; value < 48UZ; ++value) {
                        auto const x = static_cast<I>(value);
                        BOOST_CHECK_EQUAL(static_cast<std::size_t>(xstd::align_up(x, alignment)),
                                          static_cast<std::size_t>(boost::alignment::align_up(x, alignment)));
                        BOOST_CHECK_EQUAL(static_cast<std::size_t>(xstd::align_down(x, alignment)),
                                          static_cast<std::size_t>(boost::alignment::align_down(x, alignment)));
                }
        }
}

BOOST_AUTO_TEST_CASE(BoostAlignExhaustive)
{
        agrees_with_boost_align<std::uint8_t>();
        agrees_with_boost_align<std::uint16_t>();
        agrees_with_boost_align<std::uint32_t>();
        agrees_with_boost_align<std::uint64_t>();
        agrees_with_boost_align<std::size_t>();
}

BOOST_AUTO_TEST_SUITE_END()
