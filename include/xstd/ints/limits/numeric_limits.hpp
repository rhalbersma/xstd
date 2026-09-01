//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_LIMITS_NUMERIC_LIMITS_HPP
#define XSTD_LIMITS_NUMERIC_LIMITS_HPP

#include <limits>      // numeric_limits
#include <type_traits> // is_array_v, is_object_v

namespace xstd {

// The empty primary says nothing, as make_signed's does; empty and not merely declared, because the
// cv specializations below inherit it and a base must be complete.
template<class T>
struct numeric_limits
{};

// Open counterpart of std::numeric_limits, whose members an array or a function type cannot return.
template<class T>
        requires (std::is_object_v<T> and not std::is_array_v<T>)
struct numeric_limits<T> : std::numeric_limits<T>
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
