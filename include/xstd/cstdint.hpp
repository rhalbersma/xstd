//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDINT_HPP
#define XSTD_CSTDINT_HPP

#include <cstdint>

// GCC and Clang expose 128-bit integers as language-extension fundamental
// types. MSVC does not, but its standard library supplies the constexpr
// integer classes it uses internally for C++23 library facilities. These
// aliases give portable code one public spelling while retaining each
// toolchain's native representation and operator implementation.
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
namespace xstd {

using int128_t = __int128;
using uint128_t = unsigned __int128;

} // namespace xstd
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#elifdef _MSVC_STL_VERSION
#include <__msvc_int128.hpp>
namespace xstd {

using int128_t = std::_Signed128;
using uint128_t = std::_Unsigned128;

} // namespace xstd
#else
#error "xstd::int128_t requires GCC/Clang __int128 or the Microsoft STL 128-bit integer classes"
#endif

#endif // XSTD_CSTDINT_HPP
