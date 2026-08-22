//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_ABSL_INT128_HPP
#define XSTD_TEST_ABSL_INT128_HPP

// An integer-class type from outside the library, carrying no noexcept anywhere.
#if __has_include(<absl/numeric/int128.h>)

#define XSTD_TEST_HAS_ABSL_INT128

#include <absl/numeric/int128.h>    // int128, uint128
#include <xstd/ext/absl/int128.hpp> // NOLINT(misc-include-cleaner): the pair's associations, which the tests use through the traits

namespace xstd::test {

// One spelling for the tests to use, whichever header supplied it.
using absl_int128 = absl::int128;
using absl_uint128 = absl::uint128;

} // namespace xstd::test

#endif // __has_include(<absl/numeric/int128.h>)

#endif // XSTD_TEST_ABSL_INT128_HPP
