//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

#include <xstd/type_traits.hpp> // is_specialization_of_v
#include <type_traits>          // is_enum_v

namespace xstd {

// <concepts> covers the built-in numeric categories - integral,
// signed_integral, unsigned_integral, floating_point - but has nothing for
// enums, so a template that wants one has to fall back on a requires-clause
// over std::is_enum_v. Spelled as the noun the type satisfies rather than as
// is_enum, because that is how every concept in <concepts> reads (integral,
// destructible, regular), and because a concept sharing a name with the trait
// it wraps would be ambiguous under using-directives for both namespaces.
template<class T>
concept enumeration = std::is_enum_v<T>;

// The constraint spelling of xstd::is_specialization_of. The type under test
// comes first so that a partial application names the primary template alone,
// which is the form a type-constraint needs:
//
//     template<specialization_of<std::complex> T> void f(T);
//
// Primary carries the same template<class...> class restriction, and the same
// hard error for a template with a non-type parameter, as the trait does.
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<T, Primary>;

} // namespace xstd

#endif // XSTD_CONCEPTS_HPP
