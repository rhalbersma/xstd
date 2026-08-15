//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_EXACT_WIDTH_TYPES_HPP
#define XSTD_TEST_EXACT_WIDTH_TYPES_HPP

#include <xstd/test/absl_int128.hpp>  // XSTD_TEST_HAS_ABSL_INT128
#include <xstd/test/boost_int128.hpp> // XSTD_TEST_HAS_BOOST_INT128
#include <xstd/cstdint.hpp>           // int128, uint128
#include <cstdint>                    // exact-width integer types
#include <tuple>                      // tuple, tuple_cat
#include <utility>                    // declval

// The exact-width lists, one per library, so a case can name just the widths it can use.
namespace xstd::test {

// The widths the standard names, 8 through 64 bits.
using std_signed_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;
using std_unsigned_types = std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

// And the width xstd names on top of those.
using xstd_signed_types = std::tuple<int128>;
using xstd_unsigned_types = std::tuple<uint128>;

// And a 128-bit type from outside the library, when the build has one.
#ifdef XSTD_TEST_HAS_BOOST_INT128
using boost_signed_types = std::tuple<boost_int128>;
using boost_unsigned_types = std::tuple<boost_uint128>;
#else
using boost_signed_types = std::tuple<>;
using boost_unsigned_types = std::tuple<>;
#endif

// A second, conditional on an intrinsic: without one its operator/ is not constexpr.
#if defined(XSTD_TEST_HAS_ABSL_INT128) and defined(ABSL_HAVE_INTRINSIC_INT128)
using absl_signed_types = std::tuple<absl_int128>;
using absl_unsigned_types = std::tuple<absl_uint128>;
#else
using absl_signed_types = std::tuple<>;
using absl_unsigned_types = std::tuple<>;
#endif

// Concatenated through declval so the lists above stay readable as lists.
using exact_width_signed_types = decltype(std::tuple_cat(
        std::declval<std_signed_types>(), std::declval<xstd_signed_types>(), std::declval<boost_signed_types>(),
        std::declval<absl_signed_types>()));
using exact_width_unsigned_types = decltype(std::tuple_cat(
        std::declval<std_unsigned_types>(), std::declval<xstd_unsigned_types>(), std::declval<boost_unsigned_types>(),
        std::declval<absl_unsigned_types>()));

} // namespace xstd::test

#endif // XSTD_TEST_EXACT_WIDTH_TYPES_HPP
