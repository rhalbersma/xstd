//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP
#define XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP

#include <xstd/cstdint.hpp>                 // int128, uint128
#include <xstd/concepts/integer_class.hpp>  // integer_class
#include <xstd/concepts/integer_like.hpp>   // integer_like
#include <xstd/type_traits/is_unsigned.hpp> // is_unsigned_v
#include <concepts>                         // integral
#include <type_traits>                      // make_unsigned, type_identity

namespace xstd {

// Unsigned counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_unsigned
{};

// Ask std for the integral half of integer_like, which is every integral it answers for:
// bool it excludes, and an enumeration never was one, though std::make_unsigned takes it.
template<integer_like I>
        requires std::integral<I>
struct make_unsigned<I> : std::make_unsigned<I>
{};

// The other half is where std stops. Such a type is its own counterpart; a signed one
// needs a user specialization, and the empty primary is what says so.
template<integer_class I>
        requires is_unsigned_v<I>
struct make_unsigned<I> : std::type_identity<I>
{};

// The public 128-bit aliases provide their cross-direction association.
template<>
struct make_unsigned<int128> : std::type_identity<uint128>
{};

template<class T>
using make_unsigned_t = make_unsigned<T>::type;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP
