//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONFIG_INT128_HPP
#define XSTD_CONFIG_INT128_HPP

// Prefer the Microsoft STL's integer classes; otherwise use the compiler's
// 128-bit extension.
#ifdef _MSC_VER

#include <__msvc_int128.hpp>

namespace xstd {

using int128 = std::_Signed128;
using uint128 = std::_Unsigned128;

} // namespace xstd

#elifdef __SIZEOF_INT128__

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

namespace xstd {

using int128 = __int128;
using uint128 = unsigned __int128;

} // namespace xstd

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#else

#error "xstd::int128 requires GCC/Clang __int128 or the Microsoft STL 128-bit integer classes"

#endif

#endif // XSTD_CONFIG_INT128_HPP
