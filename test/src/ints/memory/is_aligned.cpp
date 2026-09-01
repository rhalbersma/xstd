//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/is_aligned.hpp> // is_aligned
#include <xstd/test/constexpr_check.hpp>   // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                        // same_as
#include <cstdint>                         // uint8_t

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Memory)
BOOST_AUTO_TEST_SUITE(IsAligned)

// The remainder being zero, which is the question the other two answer by subtracting it.
BOOST_AUTO_TEST_CASE(HoldsWhenTheRemainderIsZero)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK(     xstd::is_aligned( 0UZ, 8));
        XSTD_CONSTEXPR_CHECK(not  xstd::is_aligned( 1UZ, 8));
        XSTD_CONSTEXPR_CHECK(     xstd::is_aligned( 8UZ, 8));
        XSTD_CONSTEXPR_CHECK(not  xstd::is_aligned( 9UZ, 8));
        XSTD_CONSTEXPR_CHECK(     xstd::is_aligned(64UZ, 8));
        XSTD_CONSTEXPR_CHECK(     xstd::is_aligned( 5UZ, 1));
        // clang-format on
}

// A question about a value, not a value of the operand's own type.
BOOST_AUTO_TEST_CASE(AnswersInBool)
{
        static_assert(std::same_as<decltype(xstd::is_aligned(std::uint8_t{}, 4)), bool>);
        static_assert(std::same_as<decltype(xstd::is_aligned(0UZ, 4)), bool>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
