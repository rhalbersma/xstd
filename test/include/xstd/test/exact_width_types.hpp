//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_EXACT_WIDTH_TYPES_HPP
#define XSTD_TEST_EXACT_WIDTH_TYPES_HPP

#include <xstd/test/absl_int128.hpp>  // XSTD_TEST_HAS_ABSL_INT128
#include <xstd/test/bit_precise.hpp>  // XSTD_TEST_HAS_BIT_PRECISE, bit_int, bit_uint
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
#ifdef XSTD_TEST_HAS_ABSL_INT128
using absl_signed_types = std::tuple<absl_int128>;
using absl_unsigned_types = std::tuple<absl_uint128>;
#else
using absl_signed_types = std::tuple<>;
using absl_unsigned_types = std::tuple<>;
#endif

// Abseil falls back to non-constexpr division where the compiler has no intrinsic 128-bit type.
template<class T>
inline constexpr bool has_constexpr_division = true;

#if defined(XSTD_TEST_HAS_ABSL_INT128) and not defined(ABSL_HAVE_INTRINSIC_INT128)
template<>
inline constexpr bool has_constexpr_division<absl_int128> = false;
template<>
inline constexpr bool has_constexpr_division<absl_uint128> = false;
#endif

// Native bit-precise widths, from the two bits current Clang's signed type needs up.
#ifdef XSTD_TEST_HAS_BIT_PRECISE
// The widths every implementation of the extension divides without help from a runtime.
using narrow_bit_precise_signed_types =
        std::tuple<bit_int<8>, bit_int<16>, bit_int<32>, bit_int<64>>;
using narrow_bit_precise_unsigned_types =
        std::tuple<bit_uint<8>, bit_uint<16>, bit_uint<32>, bit_uint<64>>;

// And the two above them, each asking the ceiling rather than assuming one.
#if XSTD_TEST_BIT_PRECISE_MAX >= 128
using wide_bit_precise_signed_types = std::tuple<bit_int<128>>;
using wide_bit_precise_unsigned_types = std::tuple<bit_uint<128>>;
#else
using wide_bit_precise_signed_types = std::tuple<>;
using wide_bit_precise_unsigned_types = std::tuple<>;
#endif

#if XSTD_TEST_BIT_PRECISE_MAX >= 256
using widest_bit_precise_signed_types = std::tuple<bit_int<256>>;
using widest_bit_precise_unsigned_types = std::tuple<bit_uint<256>>;
#else
using widest_bit_precise_signed_types = std::tuple<>;
using widest_bit_precise_unsigned_types = std::tuple<>;
#endif

using bit_precise_signed_types = decltype(std::tuple_cat(
        std::declval<narrow_bit_precise_signed_types>(), std::declval<wide_bit_precise_signed_types>(),
        std::declval<widest_bit_precise_signed_types>()));
using bit_precise_unsigned_types = decltype(std::tuple_cat(
        std::declval<narrow_bit_precise_unsigned_types>(), std::declval<wide_bit_precise_unsigned_types>(),
        std::declval<widest_bit_precise_unsigned_types>()));
#else
using bit_precise_signed_types = std::tuple<>;
using bit_precise_unsigned_types = std::tuple<>;
#endif

// Constant-evaluation tests omit only Abseil's fallback implementation.
#if defined(XSTD_TEST_HAS_ABSL_INT128) and not defined(ABSL_HAVE_INTRINSIC_INT128)
using constexpr_absl_signed_types = std::tuple<>;
using constexpr_absl_unsigned_types = std::tuple<>;
#else
using constexpr_absl_signed_types = absl_signed_types;
using constexpr_absl_unsigned_types = absl_unsigned_types;
#endif
using constexpr_exact_width_signed_integer_types = decltype(std::tuple_cat(
        std::declval<std_signed_types>(), std::declval<xstd_signed_types>(), std::declval<boost_signed_types>(),
        std::declval<constexpr_absl_signed_types>(), std::declval<bit_precise_signed_types>()));
using constexpr_exact_width_unsigned_integer_types = decltype(std::tuple_cat(
        std::declval<std_unsigned_types>(), std::declval<xstd_unsigned_types>(), std::declval<boost_unsigned_types>(),
        std::declval<constexpr_absl_unsigned_types>(), std::declval<bit_precise_unsigned_types>()));

// The authoritative configured test universe. Signed and unsigned subsets remain available
// because most arithmetic contracts differ at zero or at the signed minimum.
using exact_width_signed_integer_types = decltype(std::tuple_cat(
        std::declval<std_signed_types>(), std::declval<xstd_signed_types>(), std::declval<boost_signed_types>(),
        std::declval<absl_signed_types>(), std::declval<bit_precise_signed_types>()));
using exact_width_unsigned_integer_types = decltype(std::tuple_cat(
        std::declval<std_unsigned_types>(), std::declval<xstd_unsigned_types>(), std::declval<boost_unsigned_types>(),
        std::declval<absl_unsigned_types>(), std::declval<bit_precise_unsigned_types>()));
using exact_width_integer_types = decltype(std::tuple_cat(
        std::declval<exact_width_signed_integer_types>(),
        std::declval<exact_width_unsigned_integer_types>()));

} // namespace xstd::test

#endif // XSTD_TEST_EXACT_WIDTH_TYPES_HPP
