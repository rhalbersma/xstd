//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

#include <xstd/type_traits.hpp> // is_integral_like_v, is_signed_like_v, is_specialization_of_v, is_unsigned_like_v, make_unsigned_like_t
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
// unsigned_integral, and the constraint spelling of xstd::is_integral_like_v
// - the same relation enumeration has to std::is_enum_v and specialization_of
// has to xstd::is_specialization_of_v, and the one std::integral itself has
// to std::is_integral_v.
//
// What the trait opens, and how, is <xstd/type_traits.hpp>'s business. The
// short version: <concepts>'s three are spelled over std::is_integral, which
// is a closed list the compiler owns rather than a property a type can have,
// and two kinds of type that behave like integers in every way a division or
// an absolute value cares about are outside it:
//
// - __int128, which libstdc++ withholds from std::is_integral in the strictly
//   conforming dialect (see xstd::make_unsigned_like), so the same type is
//   integral or not depending on a compiler flag;
// - integer-class types: 128-bit *class* types with the full arithmetic
//   interface and a specialized std::numeric_limits, such as libstdc++'s
//   std::ranges::__detail::__max_diff_type or the MSVC STL's std::_Signed128.
//   No class type can ever satisfy std::is_integral, on any dialect.
//
// The trait admits both, by asking what a type does rather than what it is
// called, so a user's own type qualifies by behaving correctly instead of by
// being enumerated somewhere. It is a superset of std::integral, including
// the cv-qualified spellings accepted by that standard concept.
template<class T>
concept integral_like = is_integral_like_v<T>;

// The signedness split, spelled the way <concepts> spells its own:
// std::signed_integral is integral<T> && is_signed_v<T>, and these are
// integral_like<T> and is_signed_like_v<T>. Reaching for std::is_signed_v
// directly would not do - it is is_arithmetic_v<T> && T(-1) < T(0), and
// is_arithmetic_v is false for every class type, so it would leave
// signed_integral_like rejecting precisely the integer-class types the concept
// exists to admit while still looking correct at every built-in width. That is
// what xstd::is_signed_like_v opens, without changing the question being
// asked. std::unsigned_integral negates signed_integral; after substituting
// the opened atoms and simplifying its repeated integral test, that becomes
// integral_like<T> and not is_signed_like_v<T> below.
//
// bool comes out unsigned_integral_like, exactly as std::unsigned_integral
// <bool> already holds; these concepts widen the built-in ones rather than
// tidy them up.
//
// The unsigned concept deliberately follows the standard's spelling as the
// negation of signedness, rather than being based on is_unsigned_like_v.
//
// Both are written as "integral_like<T> and ..." rather than as a flat list of
// requirements, so that each one *subsumes* integral_like. That is what lets a
// caller overload on integral_like and signed_integral_like and have the more
// constrained one win, the same way std::integral and std::signed_integral
// partial-order today; a formulation that spelled either of them over a single
// trait of its own - is_signed_integral_like_v<T>, say - would share no atomic
// constraint with integral_like and would leave such a call ambiguous instead.
//
// Which is the other reason there is deliberately no is_signed_integral_like_v
// or is_unsigned_integral_like_v to go with these two. The first is that the
// standard has no is_signed_integral_v or is_unsigned_integral_v either:
// signed_integral and unsigned_integral exist only as concepts, spelled over
// the is_signed_v / is_unsigned_v that xstd opens in <xstd/type_traits.hpp>,
// and a type only earns a _like spelling when the standard entity it widens
// exists to be widened. is_integral_like_v is a trait because std::is_integral
// is one.
template<class T>
concept unsigned_integral_like =
        integral_like<T> and
        (not is_signed_like_v<T>);

// Like std::signed_integral, this concept classifies only by integrality and
// signedness. Operations which need an unsigned counterpart constrain that
// operation separately; they are not part of the numeric category itself.
template<class T>
concept signed_integral_like =
        integral_like<T> and
        is_signed_like_v<T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_HPP
