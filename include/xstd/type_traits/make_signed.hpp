//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP
#define XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP

#include <xstd/cstdint.hpp>                // int128, uint128
#include <xstd/concepts/integer_class.hpp> // integer_class
#include <xstd/type_traits/is_signed.hpp>  // is_signed_v
#include <type_traits>                     // is_integral_v, is_same_v, make_signed, remove_cv_t, type_identity

namespace xstd {

// Signed counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_signed
{};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_signed<T> : std::make_signed<T>
{};

// Such a type is its own counterpart; an unsigned one needs a user specialization.
template<class T>
        requires integer_class<T> and is_signed_v<T>
struct make_signed<T> : std::type_identity<T>
{};

template<class T>
using make_signed_t = make_signed<T>::type;

// The public 128-bit aliases provide their cross-direction association.
template<>
struct make_signed<uint128> : std::type_identity<int128>
{};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP
