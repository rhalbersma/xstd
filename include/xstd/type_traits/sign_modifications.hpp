//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_SIGN_MODIFICATIONS_HPP
#define XSTD_TYPE_TRAITS_SIGN_MODIFICATIONS_HPP

#include <xstd/cstdint.hpp>             // int128_t, uint128_t
#include <xstd/type_traits/numeric.hpp> // is_integral_like_v, is_signed_like_v, is_unsigned_like_v
#include <type_traits>                  // is_integral_v, is_same_v, make_signed, make_unsigned, remove_cv_t, type_identity

namespace xstd {

// Signed and unsigned counterparts for integer-like types. Empty primary
// templates make unsupported associations substitution failures.
// clang-format off
template<class T>
struct make_signed_like {};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_signed_like<T> : std::make_signed<T> {};

// A signed integer-class type is its own signed counterpart. An unsigned class
// type needs a user specialization naming its signed partner.
template<class T>
        requires (not std::is_integral_v<T>) and is_integral_like_v<T> and is_signed_like_v<T>
struct make_signed_like<T> : std::type_identity<T> {};
// clang-format on

template<class T>
using make_signed_like_t = make_signed_like<T>::type;

// The unsigned half follows the same arrangement and excludes bool just as
// std::make_unsigned does.
// clang-format off
template<class T>
struct make_unsigned_like {};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_unsigned_like<T> : std::make_unsigned<T> {};

// Unsigned integer-class types are their own unsigned counterpart.
template<class T>
        requires (not std::is_integral_v<T>) and is_integral_like_v<T> and is_unsigned_like_v<T>
struct make_unsigned_like<T> : std::type_identity<T> {};
// clang-format on

template<class T>
using make_unsigned_like_t = make_unsigned_like<T>::type;

// The public 128-bit aliases provide their cross-direction association.
// clang-format off
template<>
struct make_signed_like<uint128_t> : std::type_identity<int128_t> {};

template<>
struct make_unsigned_like<int128_t> : std::type_identity<uint128_t> {};
// clang-format on

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_SIGN_MODIFICATIONS_HPP
