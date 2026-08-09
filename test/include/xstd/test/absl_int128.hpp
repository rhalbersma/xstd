//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_ABSL_INT128_HPP
#define XSTD_TEST_ABSL_INT128_HPP

// An integer-class type xstd has never heard of which carries no noexcept
// anywhere, so the conditional exception specification is answered against a
// type in the field rather than against a fixture written to imitate one.
#if __has_include(<absl/numeric/int128.h>)

#define XSTD_TEST_HAS_ABSL_INT128

#include <absl/numeric/int128.h>                   // int128, uint128
#include <xstd/type_traits/make_signed_like.hpp>   // make_signed_like
#include <xstd/type_traits/make_unsigned_like.hpp> // make_unsigned_like
#include <type_traits>                             // type_identity

namespace xstd::test {

// Named here for the reason the Boost.Int128 pair is: one spelling for the
// tests to use, whichever header supplied it.
using absl_int128 = absl::int128;
using absl_uint128 = absl::uint128;

} // namespace xstd::test

namespace xstd {

// The one thing a library cannot work out for a type it does not know. Here
// rather than in a shipped header, so this stays a test of the extension point.
template<>
struct make_unsigned_like<test::absl_int128> : std::type_identity<test::absl_uint128>
{};

template<>
struct make_signed_like<test::absl_uint128> : std::type_identity<test::absl_int128>
{};

} // namespace xstd

#endif // __has_include(<absl/numeric/int128.h>)

#endif // XSTD_TEST_ABSL_INT128_HPP
