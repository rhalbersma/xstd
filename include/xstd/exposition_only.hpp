//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_EXPOSITION_ONLY_HPP
#define XSTD_EXPOSITION_ONLY_HPP

#include <concepts>    // constructible_from, integral, regular, same_as, totally_ordered
#include <limits>      // numeric_limits
#include <type_traits> // remove_cv_t

// Entities the library needs in order to say what its own traits and concepts
// mean, and that no user of it should ever have to name - the same role the
// standard's italicized *exposition-only* names play in its specification,
// which is where this namespace takes its name. What is here is here because
// writing it down is better than describing it in a comment, not because it
// is part of the interface: it carries no stability promise, and the one
// question it answers has a public spelling as xstd::is_integral_like_v in
// <xstd/type_traits.hpp> and as xstd::integral_like in <xstd/concepts.hpp>.
//
// Both entities below are concepts rather than variable templates, and are
// spelled over the concepts in <concepts> rather than over the traits those
// are themselves spelled over - std::integral, not std::is_integral_v;
// std::same_as, not std::is_same_v. Two reasons, one of them load-bearing:
//
// - a concept's conjunction short-circuits during satisfaction checking,
//   where a variable template's initializer has to be well-formed in every
//   operand at once. That is what lets these answer "no" for an array or a
//   reference type instead of stopping the compile over
//   std::numeric_limits' primary template, and it is why the public trait
//   reads a concept rather than the other way round;
// - a constraint written in concepts partial-orders. Nothing here relies on
//   that yet, but a requirement that is one day narrowed - or reused by a
//   second concept - subsumes correctly by construction, where a conjunction
//   of trait-valued atomic constraints never would.
namespace xstd::exposition_only {

// [iterator.concept.winc] answers "what is an integer, beyond the built-in
// list" in two steps:
//
// - an *integer-class type* is defined in full detail - the operators it has
//   to provide, what they have to return, how it converts to and from the
//   integral types, and what its std::numeric_limits has to say;
// - the exposition-only concept is-integer-like is then simply "models
//   integral, or is an integer-class type".
//
// This is the first step, and is_integral_like below is the second. The first
// is where the standard's version closes: an integer-class type is one of "a
// set of implementation-defined class types", so the definition describes
// behavior but membership is by decree - libstdc++ spells its own test with
// same_as against __max_diff_type and __max_size_type, the MSVC STL against
// _Signed128 and _Unsigned128. A type outside the standard library cannot
// join, however exactly it meets the description, and nothing outside those
// headers can reuse the test.
//
// So this is that step re-derived structurally - a concept a type satisfies
// by behaving correctly rather than by being named. The requirements are the
// ones an integer-class type has that xstd asks of it, in the order they can
// safely be asked:
//
// - not integral. This is the *other* disjunct of is_integral_like below, so
//   the two partition their domain rather than overlapping, and the built-in
//   widths are answered by std::integral alone - exactly as the standard's
//   own division of labour has it.
// - a complete type. Nothing an incomplete type does is knowable, so the
//   answer is no - but it has to be *answered*, and everything after this
//   point would ask a question that requires a complete type to be well-formed
//   rather than merely false. std::regular is the first of them:
//   std::destructible is std::is_nothrow_destructible_v, which the standard
//   requires a complete type for and which the MSVC STL diagnoses outright,
//   where libstdc++ and libc++ happen to answer false. A trait that widens
//   std::is_integral_v has to cope with an incomplete type at least as well as
//   std::is_integral_v does, and it answers false for one quite happily.
// - std::regular and std::totally_ordered. B(I), the hypothetical extended
//   integer type an integer-class type behaves as, is copyable, default
//   constructible, equality-comparable and ordered, so I is too. std::regular
//   leads the rest of the conjunction for a second reason: std::numeric_limits'
//   primary template declares static member functions returning I, which for
//   an array type is ill-formed rather than merely unspecialized, so arrays,
//   references, void and function types have to be rejected before
//   numeric_limits is named on them at all.
// - std::numeric_limits<I> specialized, and saying is_integer. The standard
//   requires numeric_limits<I> to mirror numeric_limits<B(I)> member for
//   member; a specialization that reports is_integer is the part of that
//   mirroring a generic library can both check and use, and it is the marker
//   the standard itself uses for "this type is a number".
// - explicit construction from int. An integer-class type is only required to
//   be *explicitly* convertible from an integral type, which is why
//   std::constructible_from is the right question and "a < 0" is not: that
//   expression need not compile for one, while "a < static_cast<I>(0)" does.
//   Everything in <xstd/cstdlib.hpp> spells its constants the second way.
// - the six arithmetic operators, each written through static_cast<I>. The
//   built-in types promote - int8_t + int8_t is an int - so requiring the
//   operators to return I directly would exclude precisely the widths this
//   library exists to cover, while requiring the cast to land on I rather
//   than on merely something convertible is what keeps a partial imitation
//   out.
//
// What is deliberately *not* required, though [iterator.concept.winc] does
// require it of the standard's own integer-class types: ++ and --, the
// compound assignments, the bitwise and shift operators, explicit conversion
// back to any integral type, contextual conversion to bool, the common_type
// specializations, and a width greater than 64. The inclusion therefore runs
// one way and only one way - every integer-class type satisfies this concept,
// since the standard asks strictly more of one - which is the direction that
// matters: no type the standard blesses is turned away. Asking for the rest
// would turn away types that do everything xstd will ever ask of them,
// starting with the 32-bit fixture in test/include/xstd/test/integer_class.hpp
// and with any user's type that provides arithmetic without bit-twiddling. A
// concept is a promise about what will be *used*, and <xstd/cstdlib.hpp> uses
// exactly what is listed above.
template<class I>
concept integral_class_type =
        (not std::integral<I>) and
        requires { sizeof(I); } and
        std::regular<I> and
        std::totally_ordered<I> and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer and
        std::constructible_from<I, int> and
        requires (I const a, I const b) {
                { static_cast<I>(-a) } -> std::same_as<I>;
                { static_cast<I>(a + b) } -> std::same_as<I>;
                { static_cast<I>(a - b) } -> std::same_as<I>;
                { static_cast<I>(a * b) } -> std::same_as<I>;
                { static_cast<I>(a / b) } -> std::same_as<I>;
                { static_cast<I>(a % b) } -> std::same_as<I>;
        };

// [iterator.concept.winc]'s is-integer-like: an integral type, or an
// integer-class type. xstd::is_integral_like_v is this concept read as a
// value, and xstd::integral_like is this concept's public name; both are
// spelled over it rather than duplicating it.
//
// Two deliberate departures from the standard's is-integer-like:
//
// - bool is included here and excluded there (LWG 3467). xstd's concepts
//   widen std::integral, which accepts bool, and they widen it rather than
//   tidy it up: bool comes out xstd::unsigned_integral_like exactly as
//   std::unsigned_integral<bool> already holds. is-integer-like has a
//   different job - describing what an iterator's difference type may be -
//   and bool is not one of those.
// - cv-qualified types are excluded here and included there. std::integral
//   <int const> holds, but no cv-qualified type is one
//   xstd::make_unsigned_like will name a counterpart for, and no function in
//   <xstd/cstdlib.hpp> can be instantiated at one, since a by-value parameter
//   deduces its cv-qualifiers away. Excluding them keeps the domain the same
//   as make_unsigned_like's, which is restricted by the same same_as test.
//   The restriction is load-bearing for the first disjunct only: a
//   cv-qualified class type is already turned away by integral_class_type's
//   std::regular, and std::regular alone would not do the job here either -
//   it rejects int const, which is not assignable, but accepts int volatile,
//   which is.
template<class I>
concept is_integral_like =
        std::same_as<I, std::remove_cv_t<I>> and
        (std::integral<I> or integral_class_type<I>);

} // namespace xstd::exposition_only

#endif // XSTD_EXPOSITION_ONLY_HPP
