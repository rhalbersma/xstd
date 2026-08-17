//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP
#define XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP

#include <xstd/cstdint.hpp>                 // int128, uint128
#include <xstd/concepts/integer_class.hpp>  // integer_class
#include <xstd/type_traits/is_unsigned.hpp> // is_unsigned_v
#include <type_traits>                      // is_integral_v, is_same_v, make_unsigned, remove_cv_t, type_identity

namespace xstd {

// Unsigned counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_unsigned
{};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_unsigned<T> : std::make_unsigned<T>
{};

// Such a type is its own counterpart; a signed one needs a user specialization.
template<class T>
        requires integer_class<T> and is_unsigned_v<T>
struct make_unsigned<T> : std::type_identity<T>
{};

template<class T>
using make_unsigned_t = make_unsigned<T>::type;

// The public 128-bit aliases provide their cross-direction association.
template<>
struct make_unsigned<int128> : std::type_identity<uint128>
{};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP
