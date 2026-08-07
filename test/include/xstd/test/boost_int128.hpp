//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BOOST_INT128_HPP
#define XSTD_TEST_BOOST_INT128_HPP

// Boost.Int128 is an integer-class type that xstd has never heard of: no
// shipped header names it, and nothing specializes anything for it. That is
// what it is here for. xstd::int128 is an alias for whichever 128-bit type the
// platform has, and the library provides its trait associations itself, so it
// can only ever confirm that the concepts admit a type xstd already caters
// for. Boost.Int128 arrives the way a third party's type does, which is the
// property the widening is actually claiming.
//
// It ships with Boost 1.92; until then test/CMakeLists.txt fetches it. Where
// neither route provides it, the test suite falls back to xstd's own types.
#if __has_include(<boost/int128.hpp>)

#define XSTD_TEST_HAS_BOOST_INT128

#include <boost/int128.hpp>                        // int128, uint128
#include <xstd/type_traits/make_signed_like.hpp>   // make_signed_like
#include <xstd/type_traits/make_unsigned_like.hpp> // make_unsigned_like
#include <type_traits>                             // type_identity

namespace xstd::test {

// Named once, here, because the spelling is not settled: upstream dropped the
// _t suffix from both after its most recent release tag, so which one is
// correct depends on the commit test/CMakeLists.txt pins.
using boost_int128 = boost::int128::int128;
using boost_uint128 = boost::int128::uint128;

} // namespace xstd::test

namespace xstd {

// The one thing a library cannot work out for a type it does not know. Every
// other requirement an integer-class type has to meet is a property of its own
// operations, which the concepts check directly; which type is its signed or
// unsigned counterpart is a fact only its author or its user can state. These
// two specializations are the whole of what Boost.Int128 needs to reach the
// same facilities the built-in widths do - and they are written here, in the
// tests, rather than in a shipped header, so that this remains a test of the
// extension point rather than a dependency on Boost.Int128.
template<>
struct make_unsigned_like<test::boost_int128> : std::type_identity<test::boost_uint128>
{};

template<>
struct make_signed_like<test::boost_uint128> : std::type_identity<test::boost_int128>
{};

} // namespace xstd

#endif // __has_include(<boost/int128.hpp>)

#endif // XSTD_TEST_BOOST_INT128_HPP
