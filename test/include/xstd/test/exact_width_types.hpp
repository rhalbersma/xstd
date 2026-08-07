//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_EXACT_WIDTH_TYPES_HPP
#define XSTD_TEST_EXACT_WIDTH_TYPES_HPP

#include <xstd/test/boost_int128.hpp> // XSTD_TEST_HAS_BOOST_INT128
#include <xstd/cstdint.hpp>           // int128, uint128
#include <cstdint>                    // exact-width integer types
#include <tuple>                      // tuple, tuple_cat
#include <utility>                    // declval

// The lists the exact-width test cases run over. Shared rather than repeated
// per file: which types those cases cover is one decision, and a type added to
// the suite should reach every case that a width can be plugged into.
namespace xstd::test {

// The widths xstd names itself, 8 through 128 bits.
using xstd_signed_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t, int128>;
using xstd_unsigned_types = std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, uint128>;

// And a 128-bit type from outside the library, when the build has one.
#ifdef XSTD_TEST_HAS_BOOST_INT128
using third_party_signed_types = std::tuple<boost_int128>;
using third_party_unsigned_types = std::tuple<boost_uint128>;
#else
using third_party_signed_types = std::tuple<>;
using third_party_unsigned_types = std::tuple<>;
#endif

// Concatenated through declval rather than by splicing the conditional entry
// into a braced list: the lists above stay readable as lists, and the one
// conditional stays in the one place above.
using exact_width_signed_types = decltype(std::tuple_cat(std::declval<xstd_signed_types>(), std::declval<third_party_signed_types>()));
using exact_width_unsigned_types = decltype(std::tuple_cat(std::declval<xstd_unsigned_types>(), std::declval<third_party_unsigned_types>()));

} // namespace xstd::test

#endif // XSTD_TEST_EXACT_WIDTH_TYPES_HPP
