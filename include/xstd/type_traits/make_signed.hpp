//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP
#define XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP

#include <xstd/concepts/integer_class.hpp> // integer_class
#include <xstd/type_traits/is_signed.hpp>  // is_signed_v
#include <concepts>                        // integral, same_as
#include <type_traits>                     // is_enum_v, make_signed, remove_cv_t, type_identity

namespace xstd {

// Signed counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_signed
{};

// [meta.trans.sign]/2's whole mandated domain, less the cv bool std declines to pair there.
template<class T>
        requires (std::integral<T> or std::is_enum_v<T>) and (not std::same_as<std::remove_cv_t<T>, bool>)
struct make_signed<T> : std::make_signed<T>
{};

// Where std stops: its own counterpart, the other half of the pair being the user's to say.
template<integer_class I>
        requires is_signed_v<I>
struct make_signed<I> : std::type_identity<I>
{};

template<class T>
using make_signed_t = make_signed<T>::type;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_SIGNED_HPP
