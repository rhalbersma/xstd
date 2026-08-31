//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_PROMOTED_HPP
#define XSTD_TYPE_TRAITS_PROMOTED_HPP

#include <concepts> // integral
#include <utility>  // declval

namespace xstd {

// The type T's own operators yield, asked of the language rather than modelled.
template<class T>
struct promoted
{
        using type = T;
};

// [conv.prom] for a built-in; the primary above leaves every other type alone.
template<std::integral T>
struct promoted<T>
{
        using type = decltype(+std::declval<T const&>());
};

template<class T>
using promoted_t = promoted<T>::type;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_PROMOTED_HPP
