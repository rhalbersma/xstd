//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BIT_PRECISE_HPP
#define XSTD_TEST_BIT_PRECISE_HPP

#include <xstd/ints/cstdint/bit_int.hpp> // XSTD_HAS_BIT_INT, bit_int, bit_uint

#ifdef XSTD_HAS_BIT_INT

#include <cstddef> // size_t

#define XSTD_TEST_HAS_BIT_PRECISE

// Dividing wider than 64 bits calls compiler-rt, which clang-cl does not link: __udivti3.
#ifdef _MSC_VER
#define XSTD_TEST_BIT_PRECISE_MAX 64
#elif __BITINT_MAXWIDTH__ >= 256
#define XSTD_TEST_BIT_PRECISE_MAX 256
#else
#define XSTD_TEST_BIT_PRECISE_MAX 128
#endif

namespace xstd::test {

template<std::size_t N>
using bit_int = xstd::bit_int<N>;

template<std::size_t N>
using bit_uint = xstd::bit_uint<N>;

} // namespace xstd::test

#endif // XSTD_HAS_BIT_INT

#endif // XSTD_TEST_BIT_PRECISE_HPP
