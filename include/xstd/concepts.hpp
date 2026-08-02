//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

#include <xstd/type_traits.hpp> // is_integral_like_v, is_signed_like_v, is_specialization_of_v, is_unsigned_like_v, make_unsigned_like_t
#include <concepts>             // constructible_from
#include <type_traits>          // bool_constant, is_enum_v, is_nothrow_constructible_v

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
// being enumerated somewhere. It is a strict superset of std::integral at
// every cv-unqualified type; the cv-qualified spellings std::integral also
// accepts are outside its domain, deliberately and for the reasons given
// where it is defined.
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

// The same question integral_like asks, asked again of every operation it
// requires: can any of them throw?
//
// For a built-in type the answer is always yes-they-are-noexcept, and this
// trait is a constant true. It exists for the other kind: an integer-class
// type is under no obligation to be non-throwing, and
// Boost.Multiprecision's operator/ really does throw std::overflow_error on a
// zero divisor. <xstd/cstdlib.hpp> uses these two to say noexcept exactly
// rather than approximately - unconditionally noexcept would be a promise the
// library cannot keep on such a type's behalf, and dropping noexcept
// altogether would give up a guarantee that is true at every built-in width.
//
// A note on the names, since this file argues elsewhere that _like belongs
// only to entities widening a standard one, and that there is deliberately no
// is_signed_integral_like_v. Neither of these is a widening: the standard has
// no is_nothrow_integral to open. They are the nothrow *companions* of
// is_integral_like and of signed_integral_like, in the sense that
// std::is_nothrow_constructible is the companion of std::is_constructible -
// the _like travels with the notion they are derived from. And the signed one
// earns a name here where a plain is_signed_integral_like_v did not, because
// it is not a second spelling of a concept that already exists: there is no
// nothrow concept for it to duplicate.
template<class T>
inline constexpr auto is_nothrow_integral_like_v = false;

// The static_casts match, operator for operator, the requirement set
// integral_like reaches through its trait - xstd::exposition_only::
// integral_class_type - and are load-bearing for the same reason they are
// there: an operator may return something that is not yet a T - a promoted int
// for the narrow widths, an expression-template proxy for a type like
// Boost.Multiprecision's - and materialising that is itself an operation that
// can throw. clang-tidy sees only the instantiations where the operator
// already returns T and calls the cast redundant; it is redundant for those
// and necessary for the others, which is what a template is for. It does not
// raise this against the identical casts in that concept, which sit inside a
// concept rather than a variable template.
//
// NOLINTBEGIN(readability-redundant-casting)
template<integral_like T>
inline constexpr auto is_nothrow_integral_like_v<T> = requires (T const a, T const b) {
        { static_cast<T>(0) } noexcept;
        { static_cast<T>(-a) } noexcept;
        { static_cast<T>(a + b) } noexcept;
        { static_cast<T>(a - b) } noexcept;
        { static_cast<T>(a * b) } noexcept;
        { static_cast<T>(a / b) } noexcept;
        { static_cast<T>(a % b) } noexcept;
        { a < b } noexcept;
        { a == b } noexcept;
};
// NOLINTEND(readability-redundant-casting)

template<class T>
using is_nothrow_integral_like = std::bool_constant<is_nothrow_integral_like_v<T>>;

// Everything above, for both halves of the signed/unsigned pair, plus the
// conversion between them: exactly the set of operations <xstd/cstdlib.hpp>
// performs. uabs converts T to its counterpart and does the arithmetic there,
// and the division family's postconditions run through uabs, so no function in
// that header touches one type without the other.
template<class T>
inline constexpr auto is_nothrow_signed_integral_like_v = false;

template<signed_integral_like T>
inline constexpr auto is_nothrow_signed_integral_like_v<T> =
        is_nothrow_integral_like_v<T> and
        is_nothrow_integral_like_v<make_unsigned_like_t<T>> and
        std::is_nothrow_constructible_v<make_unsigned_like_t<T>, T>;

template<class T>
using is_nothrow_signed_integral_like = std::bool_constant<is_nothrow_signed_integral_like_v<T>>;

} // namespace xstd

#endif // XSTD_CONCEPTS_HPP
