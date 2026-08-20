//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_LIMITS_NUMERIC_LIMITS_HPP
#define XSTD_LIMITS_NUMERIC_LIMITS_HPP

#include <limits>      // numeric_limits
#include <type_traits> // conditional_t, is_array_v, is_object_v

namespace xstd {

// Open counterpart of std::numeric_limits: the standard domain delegates there,
// while extension types can specialize this template in namespace xstd.
// A type std::numeric_limits cannot be instantiated for delegates as void, which
// answers the standard's unspecialized defaults instead of failing to compile.
template<class T>
struct numeric_limits : std::numeric_limits<std::conditional_t<std::is_object_v<T> and not std::is_array_v<T>, T, void>>
{};

template<class T>
struct numeric_limits<T const> : numeric_limits<T>
{};

template<class T>
struct numeric_limits<T volatile> : numeric_limits<T>
{};

template<class T>
struct numeric_limits<T const volatile> : numeric_limits<T>
{};

} // namespace xstd

#endif // XSTD_LIMITS_NUMERIC_LIMITS_HPP
