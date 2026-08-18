//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/memory.hpp>               // aligned_size
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <cstddef>                       // size_t
#include <limits>                        // numeric_limits

BOOST_AUTO_TEST_SUITE(Memory)

BOOST_AUTO_TEST_CASE(AlignedSize)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8,  0),  0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8,  1),  8);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8,  8),  8);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8,  9), 16);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8, 64), 64);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8, 65), 72);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8, std::numeric_limits<std::size_t>::max() - 7), std::numeric_limits<std::size_t>::max() - 7);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::aligned_size(8, std::numeric_limits<std::size_t>::max() - 8), std::numeric_limits<std::size_t>::max() - 7);
        // clang-format on
}

BOOST_AUTO_TEST_SUITE_END()
