//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

#include <xstd/type_traits.hpp> // is_arithmetic_like_v, is_signed_like_v, is_specialization_of_v, is_unsigned_like_v, make_unsigned_like_t
#include <concepts>             // constructible_from, regular, same_as, totally_ordered
#include <limits>               // numeric_limits
#include <type_traits>          // bool_constant, is_enum_v

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
//   conforming dialect (see xstd::make_unsigned_like), so the same type is
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
concept integral_like =
        std::regular<T> and
        std::totally_ordered<T> and
        is_arithmetic_like_v<T> and
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

// The signedness split, spelled the way <concepts> spells its own:
// std::signed_integral is integral<T> && is_signed_v<T>, and these are
// integral_like<T> and is_signed_like_v<T>. Reaching for std::is_signed_v
// directly would not do - it is is_arithmetic_v<T> && T(-1) < T(0), and
// is_arithmetic_v is false for every class type, so it would leave
// signed_integral_like rejecting precisely the integer-class types the concept
// exists to admit while still looking correct at every built-in width. That is
// what xstd::is_signed_like_v opens, without changing the question being
// asked.
//
// bool comes out unsigned_integral_like, exactly as std::unsigned_integral
// <bool> already holds; these concepts widen the built-in ones rather than
// tidy them up.
//
// Both are written as "integral_like<T> and ..." rather than as a flat list of
// requirements, so that each one *subsumes* integral_like. That is what lets a
// caller overload on integral_like and signed_integral_like and have the more
// constrained one win, the same way std::integral and std::signed_integral
// partial-order today; a formulation that repeated the requirements instead
// would leave such a call ambiguous.
template<class T>
concept unsigned_integral_like =
        integral_like<T> and
        is_unsigned_like_v<T>;

// A signed integer-like type is one that also has somewhere to put a
// magnitude that its own range cannot hold: |MIN| is one past MAX at every
// width, so an unsigned counterpart is part of what makes a signed integer
// usable, not an optional extra. Requiring it here is what lets xstd::uabs be
// total, and what makes the division family's postconditions checkable on a
// denominator of MIN.
template<class T>
concept signed_integral_like =
        integral_like<T> and
        is_signed_like_v<T> and
        requires { typename make_unsigned_like_t<T>; } and
        unsigned_integral_like<make_unsigned_like_t<T>> and
        std::constructible_from<make_unsigned_like_t<T>, T>;

// std::is_integral, opened - the trait spelling of integral_like, for generic
// code that wants a value rather than a constraint (std::conjunction, a
// tag-dispatch bool_constant, an if constexpr over a pack).
//
// This is the last of the four. There is deliberately no
// is_signed_integral_like_v or is_unsigned_integral_like_v to go with the two
// concepts below it, because the standard has no is_signed_integral_v or
// is_unsigned_integral_v either: signed_integral and unsigned_integral exist
// only as concepts, spelled over the is_signed_v / is_unsigned_v that xstd
// opens in <xstd/type_traits.hpp>. A type only earns a _like spelling here
// when the standard entity it widens exists to be widened.
//
// Unlike the three in <xstd/type_traits.hpp>, this one reads its concept
// rather than the other way round, and that direction is not free to reverse.
// integral_like's requirements include a requires-expression and a
// std::numeric_limits member; written out as a variable template's initializer
// they would all have to be well-formed at once, so is_integral_like_v<int[3]>
// would stop the compile instead of answering false. A concept's conjunction
// short-circuits during satisfaction checking, so reading one keeps the trait
// total. The three in <xstd/type_traits.hpp> reach the same safety through a
// constrained partial specialization, which is what a variable template has
// instead; integral_like is the one whose requirements do not fit in a
// requires-clause.
template<class T>
inline constexpr auto is_integral_like_v = integral_like<T>;

template<class T>
using is_integral_like = std::bool_constant<is_integral_like_v<T>>;

} // namespace xstd

#endif // XSTD_CONCEPTS_HPP
