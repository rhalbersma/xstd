//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP
#define XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP

#include <xstd/concepts/integer_class_operations.hpp> // integer_class_operations
#include <xstd/type_traits/is_unsigned.hpp>           // is_unsigned_v
#include <concepts>                                   // integral, same_as
#include <type_traits>                                // make_unsigned, remove_cv_t, type_identity

namespace xstd {

// Unsigned counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_unsigned
{};

// Ask std where std knows: its own mandated domain, less the cv bool integer_like also excludes.
template<class T>
        requires std::integral<T> and (not std::same_as<std::remove_cv_t<T>, bool>)
struct make_unsigned<T> : std::make_unsigned<T>
{};

// Where std stops: its own counterpart, the other half of the pair being the user's to say.
template<integer_class_operations I>
        requires is_unsigned_v<I>
struct make_unsigned<I> : std::type_identity<I>
{};

template<class T>
using make_unsigned_t = make_unsigned<T>::type;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_UNSIGNED_HPP
