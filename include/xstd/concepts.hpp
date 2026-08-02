//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

#include <xstd/type_traits.hpp> // is_specialization_of_v, unsigned_counterpart_t
#include <concepts>             // constructible_from, regular, same_as, totally_ordered
#include <limits>               // numeric_limits
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

// The open counterpart of <concepts>'s integral / signed_integral /
// unsigned_integral. Those three are spelled over std::is_integral, which is
// a closed list the compiler owns rather than a property a type can have, and
// two kinds of type that behave like integers in every way a division or an
// absolute value cares about are outside it:
//
// - __int128, which libstdc++ withholds from std::is_integral in the strictly
//   conforming dialect (see xstd::unsigned_counterpart), so the same type is
//   integral or not depending on a compiler flag;
// - integer-class types: 128-bit *class* types with the full arithmetic
//   interface and a specialized std::numeric_limits, such as libstdc++'s
//   std::ranges::__detail::__max_diff_type or the MSVC STL's std::_Signed128.
//   No class type can ever satisfy std::is_integral, on any dialect.
//
// The standard ran into the same wall and answered it with integer-class
// types ([iterator.concept.winc]) - but that answer is another closed list,
// spelled with same_as against each implementation's own reserved names, so
// it can be neither reused nor joined from outside. This is the structural
// version of the same question: it asks what a type does rather than what it
// is called, so a user's own type qualifies by behaving correctly instead of
// by being enumerated somewhere. It is a strict superset of std::integral.
//
// std::regular leads the conjunction so that arrays, references, void and
// function types are rejected before std::numeric_limits is instantiated over
// them: its primary template declares a static member function returning T,
// which for an array type is ill-formed rather than merely unspecialized.
//
// Every arithmetic result is written through static_cast<T> because the
// built-in types promote - int8_t + int8_t is an int - so requiring the
// operators to return T directly would exclude precisely the widths the
// library exists to cover. Requiring the cast to land on T rather than merely
// something convertible is what keeps a partial imitation out.
template<class T>
concept integer_like =
        std::regular<T> and
        std::totally_ordered<T> and
        std::numeric_limits<T>::is_specialized and
        std::numeric_limits<T>::is_integer and
        std::constructible_from<T, int> and
        requires (T const a, T const b) {
                { static_cast<T>(-a) } -> std::same_as<T>;
                { static_cast<T>(a + b) } -> std::same_as<T>;
                { static_cast<T>(a - b) } -> std::same_as<T>;
                { static_cast<T>(a * b) } -> std::same_as<T>;
                { static_cast<T>(a / b) } -> std::same_as<T>;
                { static_cast<T>(a % b) } -> std::same_as<T>;
        };

// std::constructible_from<T, int> above, rather than a comparison or an
// arithmetic operator taking a literal, is deliberate: an integer-class type
// is only required to be *explicitly* convertible from an integral type
// ([iterator.concept.winc]), so "x < 0" need not compile for one even though
// "x < static_cast<T>(0)" does. Everything in <xstd/cstdlib.hpp> spells its
// constants that second way for that reason.

// The signedness split, taken from std::numeric_limits rather than from
// std::is_signed, which is another trait no class type can satisfy. bool
// comes out unsigned_integer_like, exactly as std::unsigned_integral<bool>
// already holds; these concepts widen the built-in ones rather than tidy them
// up.
template<class T>
concept unsigned_integer_like =
        integer_like<T> and
        not std::numeric_limits<T>::is_signed;

// A signed integer-like type is one that also has somewhere to put a
// magnitude that its own range cannot hold: |MIN| is one past MAX at every
// width, so an unsigned counterpart is part of what makes a signed integer
// usable, not an optional extra. Requiring it here is what lets xstd::uabs be
// total, and what makes the division family's postconditions checkable on a
// denominator of MIN.
template<class T>
concept signed_integer_like =
        integer_like<T> and
        std::numeric_limits<T>::is_signed and
        requires { typename unsigned_counterpart_t<T>; } and
        unsigned_integer_like<unsigned_counterpart_t<T>> and
        std::constructible_from<unsigned_counterpart_t<T>, T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_HPP
