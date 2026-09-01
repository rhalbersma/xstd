//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/core/concepts/alignable.hpp> // alignable
#include <boost/test/unit_test.hpp>         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <cstddef>                          // size_t
#include <cstdint>                          // exact-width integer types

// The wide types by their built-in spelling, so this stays clear of xstd::ints.
#if defined(__SIZEOF_INT128__)
#define XSTD_TEST_HAS_INT128
#endif

// Reached the way a consumer reaches it, and directly: nothing here goes through xstd::ints.
#if __has_include(<absl/numeric/int128.h>)
#define XSTD_TEST_HAS_ABSL_INT128
#include <absl/numeric/int128.h> // uint128
#endif

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(AlignableAdmitsTheUnsignedIntegers)
{
        static_assert(xstd::alignable<unsigned char>);
        static_assert(xstd::alignable<unsigned short>);
        static_assert(xstd::alignable<unsigned>);
        static_assert(xstd::alignable<unsigned long>);
        static_assert(xstd::alignable<unsigned long long>);
        static_assert(xstd::alignable<std::size_t>);
        static_assert(xstd::alignable<std::uint8_t>);
        static_assert(xstd::alignable<std::uint64_t>);
#ifdef XSTD_TEST_HAS_INT128
        static_assert(xstd::alignable<__uint128_t>);
#endif
#ifdef __BITINT_MAXWIDTH__
        static_assert(xstd::alignable<unsigned _BitInt(8)>);
        static_assert(xstd::alignable<unsigned _BitInt(24)>);
        static_assert(xstd::alignable<unsigned _BitInt(64)>);
#endif
        BOOST_CHECK(true);
}

// Nothing is excluded by name: each of these fails a clause it cannot satisfy.
BOOST_AUTO_TEST_CASE(AndRejectsTheRestOnTheirMerits)
{
        // Not modular: zero decremented is below zero, not the largest value.
        static_assert(not xstd::alignable<signed char>);
        static_assert(not xstd::alignable<short>);
        static_assert(not xstd::alignable<int>);
        static_assert(not xstd::alignable<long long>);
#ifdef XSTD_TEST_HAS_INT128
        static_assert(not xstd::alignable<__int128_t>);
#endif
#ifdef __BITINT_MAXWIDTH__
        static_assert(not xstd::alignable<signed _BitInt(24)>);
#endif
        // Saturating rather than counting: one plus one converts back to one.
        static_assert(not xstd::alignable<bool>);
        // No ~, and for the pointers no & either, which is Boost.Align's not_pointer for free.
        static_assert(not xstd::alignable<double>);
        static_assert(not xstd::alignable<void*>);
        static_assert(not xstd::alignable<std::uint8_t*>);
        // Nothing at all.
        struct udt
        {};
        static_assert(not xstd::alignable<udt>);
        static_assert(not xstd::alignable<void>);
        BOOST_CHECK(true);
}

// Only visible on a class type without noexcept, which Abseil is: it declares noexcept nowhere.
BOOST_AUTO_TEST_CASE(NothrowAlignableRefinesIt)
{
        static_assert(xstd::nothrow_alignable<unsigned char>);
        static_assert(xstd::nothrow_alignable<std::size_t>);
#ifdef XSTD_TEST_HAS_INT128
        static_assert(xstd::nothrow_alignable<__uint128_t>);
#endif
#ifdef XSTD_TEST_HAS_ABSL_INT128
        static_assert(xstd::alignable<absl::uint128>);
        static_assert(not xstd::nothrow_alignable<absl::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
