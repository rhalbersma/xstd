//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP
#define XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP

#include <xstd/cstdint.hpp>                // int128, uint128
#include <xstd/concepts/integer_class.hpp> // integer_class
#include <xstd/concepts/integer_like.hpp>  // integer_like
#include <xstd/type_traits/is_signed.hpp>  // is_signed_v
#include <concepts>                        // integral
#include <type_traits>                     // make_signed, type_identity

namespace xstd {

// Signed counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_signed
{};

// Ask std for the integral half of integer_like, which is every integral it answers for:
// bool it excludes, and an enumeration never was one, though std::make_signed takes it.
template<integer_like I>
        requires std::integral<I>
struct make_signed<I> : std::make_signed<I>
{};

// The other half is where std stops. Such a type is its own counterpart; an unsigned one
// needs a user specialization, and the empty primary is what says so.
template<integer_class I>
        requires is_signed_v<I>
struct make_signed<I> : std::type_identity<I>
{};

// The public 128-bit aliases provide their cross-direction association.
template<>
struct make_signed<uint128> : std::type_identity<int128>
{};

template<class T>
using make_signed_t = make_signed<T>::type;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP
