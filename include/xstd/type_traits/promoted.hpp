//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_PROMOTED_HPP
#define XSTD_TYPE_TRAITS_PROMOTED_HPP

#include <concepts>    // integral
#include <type_traits> // conditional_t, type_identity
#include <utility>     // declval

namespace xstd {

// Held apart because std::conditional forms both type arguments before selecting one.
template<class T>
struct integral_promoted
{
        using type = decltype(+std::declval<T const&>());
};

// What T's own operators yield: [conv.prom] for a built-in, T itself otherwise.
template<class T>
struct promoted : std::conditional_t<std::integral<T>, integral_promoted<T>, std::type_identity<T>>
{};

template<class T>
using promoted_t = promoted<T>::type;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_PROMOTED_HPP
