//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

#include <xstd/cstdint.hpp>         // int128_t, uint128_t
#include <xstd/exposition_only.hpp> // is_integral_like
#include <compare>                  // strong_ordering (empty_type's defaulted <=>)
#include <type_traits>              // bool_constant, conditional_t, integral_constant, is_floating_point_v, is_integral_v, is_same_v, make_signed, make_unsigned, remove_cv_t, remove_cvref_t, type_identity

namespace xstd {

// std::is_integral, opened - and the root the other three are built on, in
// place of the root the standard's own four have. std::is_arithmetic_v is
// where the closed list starts for the standard (is_integral_v and
// is_floating_point_v feed it, is_signed_v and is_unsigned_v are spelled over
// it), but it is not where it can be opened: what a type has to *do* to count
// as an integer is answerable, while "arithmetic" on its own is not. So this
// is the one new judgement, and the three below follow from it by copying the
// standard's own definitions with the opened test substituted in.
//
// What it extends std::is_integral_v with is [iterator.concept.winc]'s other
// half - an integer-class type - opened from the standard's closed list of
// implementation-defined names to a structural concept, so that __int128 on a
// dialect that withholds std::is_integral from it, an implementation's own
// integer-class type, and a user's own extended-precision type all qualify by
// behaving correctly rather than by being enumerated somewhere.
//
// It reads a concept rather than being spelled out as an initializer, and that
// direction is not free to reverse. The requirements it reaches include a
// requires-expression and several std::numeric_limits members; written out
// here they would all have to be well-formed at once, so
// is_integral_like_v<int[3]> would stop the compile instead of answering
// false. A concept's conjunction short-circuits during satisfaction checking,
// which is what keeps this total, and <xstd/exposition_only.hpp> is where the
// requirements it short-circuits over are spelled out.
template<class T>
inline constexpr auto is_integral_like_v = exposition_only::is_integral_like<T>;

template<class T>
using is_integral_like = std::bool_constant<is_integral_like_v<T>>;

// std::is_arithmetic, opened - and spelled the way the standard spells it,
// as the disjunction of its two halves: an integral type, or a floating-point
// one. Only the first half is open, which is the whole content of this trait:
// xstd extends integers, so an integer-like type is arithmetic-like, and the
// floating-point half is std::is_floating_point_v unchanged.
//
// The shape is what matters more than the current answer. A class type that
// behaves like a *floating-point* number - an extended-precision binary float,
// a decimal type - is as real a case as the integer-class types, and opening
// it means a second exposition-only concept and a second disjunct here, with
// nothing above or below this line needing to move. Until then, a class type
// whose std::numeric_limits says it is not an integer is deliberately not
// arithmetic-like: xstd has opened no notion under which it would be, and
// answering yes would leave is_signed_like_v below reporting a sign for a type
// no concept in this library accepts.
//
template<class T>
inline constexpr auto is_arithmetic_like_v =
        is_integral_like_v<T> or
        std::is_floating_point_v<T>;

template<class T>
using is_arithmetic_like = std::bool_constant<is_arithmetic_like_v<T>>;

// std::is_signed and std::is_unsigned, opened - and spelled exactly the way
// the standard spells them ([meta.unary.comp]): is_arithmetic_v<T> && T(-1) <
// T(0), and is_arithmetic_v<T> && T(0) < T(-1), with the opened arithmetic
// test in place of the closed one, and nothing else changed. Asking the type
// rather than reading std::numeric_limits<T>::is_signed is what keeps them
// definitionally identical to the standard's, so a built-in answer can never
// drift from std::is_signed_v.
//
// The unsigned one is the standard's *reversed comparison*, not "arithmetic
// and not signed" - which is how libstdc++ happens to implement it, and which
// is a different question wherever T(-1) and T(0) compare equal. No type
// either of them can be asked about today is such a type, so the two agree
// everywhere it matters; the standard's spelling is the one that keeps
// answering correctly - neither signed nor unsigned - if one ever turns up.
//
// The guard is the standard's own and nothing more. An earlier revision needed
// two extra terms, std::constructible_from<T, int> and strong three-way ordering
// <T>, because is_arithmetic_like_v was then a std::numeric_limits reading and
// a class type can specialize numeric_limits without being constructible from
// int or ordered - which would make forming T(-1) a hard error rather than a
// false. Both are implied now that arithmetic-like means integer-like or
// floating-point: xstd::exposition_only::integral_class_type asks a class type
// for exactly those two, and every built-in arithmetic type has them.
//
// One note for authors of arithmetic-like class types: static_cast<T>(-1) is a
// constructor call, so a type whose only viable constructor takes an *unsigned*
// parameter converts the -1 implicitly and draws -Wsign-conversion here, at
// these lines, inside this header. Giving the type a constructor from a signed
// integral type - which [iterator.concept.winc] asks of integer-class types
// anyway - is what avoids it. There is deliberately no pragma silencing it
// from xstd's side: the conversion really is happening in the user's type, and
// the diagnostic is pointing at something worth fixing there.
template<class T>
inline constexpr auto is_signed_like_v = false;

template<class T>
        requires is_arithmetic_like_v<T>
// Both checks fire only on the bool instantiation, where -1 and 0 are the
// standard's own spelling of this test and turning them into bool literals
// would change what is being asked. T is a template parameter, so neither
// diagnostic is about the code as written.
// NOLINTNEXTLINE(readability-implicit-bool-conversion,modernize-use-bool-literals)
inline constexpr auto is_signed_like_v<T> = static_cast<T>(-1) < static_cast<T>(0);

template<class T>
using is_signed_like = std::bool_constant<is_signed_like_v<T>>;

template<class T>
inline constexpr auto is_unsigned_like_v = false;

template<class T>
        requires is_arithmetic_like_v<T>
// NOLINTNEXTLINE(readability-implicit-bool-conversion,modernize-use-bool-literals)
inline constexpr auto is_unsigned_like_v<T> = static_cast<T>(0) < static_cast<T>(-1);

template<class T>
using is_unsigned_like = std::bool_constant<is_unsigned_like_v<T>>;

template<class T, class U>
inline constexpr auto is_integral_constant_v = false;

template<class U, U N>
inline constexpr auto is_integral_constant_v<std::integral_constant<U, N>, U> = true;

template<class T, class U>
using is_integral_constant = std::bool_constant<is_integral_constant_v<T, U>>;

template<class T, template<class...> class Primary>
inline constexpr auto is_specialization_of_v = false;

template<template<class...> class Primary, class... Args>
inline constexpr auto is_specialization_of_v<Primary<Args...>, Primary> = true;

template<class T, template<class...> class Primary>
using is_specialization_of = std::bool_constant<is_specialization_of_v<T, Primary>>;

// The signed and unsigned types that pair with an integer-like type: what
// std::make_signed and std::make_unsigned answer, over the types xstd can
// answer for. Both empty primary templates make an unsupported association a
// substitution failure, and both forward successful answers by inheritance in
// the Boost.MPL metafunction-forwarding style.
template<class T>
struct make_signed_like {};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_signed_like<T> : std::make_signed<T> {};

// A signed integer-class type is its own signed counterpart. An unsigned class
// type needs a user specialization naming its signed partner.
template<class T>
        requires (not std::is_integral_v<T>) and is_integral_like_v<T> and is_signed_like_v<T>
struct make_signed_like<T> : std::type_identity<T> {};

template<class T>
using make_signed_like_t = make_signed_like<T>::type;

// The unsigned half follows the same arrangement.
//
// The domain is every integer-like type except bool, which is std::make_unsigned's
// own domain opened to the integer-class types. Two specializations cover it,
// and the split is exactly the standard's:
//
// - a built-in integral type other than bool answers what std::make_unsigned
//   answers, signed or unsigned alike. std::make_unsigned_t<unsigned> is
//   unsigned, not an error, and neither is this;
// - an unsigned integer-*class* type is its own counterpart, for the same
//   reason and with the same answer. Its author writes no specialization at
//   all: only a *signed* class type has a counterpart the compiler cannot
//   work out, and that is the one line a user of such a type still writes.
//
// bool is left out because std::make_unsigned leaves it out ([meta.trans.sign]
// asks for an integral type other than cv bool), and enumerations are left out
// because nothing in xstd calls one integer-like.
//
// std::make_unsigned is closed in two separate ways. Its domain is the
// built-in integral types other than bool, plus the enumerations - a list no
// class type can join. And outside that domain it is ill-formed rather than
// empty, so naming it on an unconstrained template parameter is a hard error
// instead of a substitution failure, which is precisely what a concept cannot
// survive: the check that was supposed to answer "no" stops the compile
// instead. Hence the deliberately empty primary template here.
// make_unsigned_like_t<T> is then a substitution failure - detectable, not
// fatal - for every type outside the domain, and a user can bring a signed
// type of their own into it by specializing. Operations such as xstd::uabs
// use that detectable association in their own interface.
//
// As with std::make_unsigned, the built-in specialization accepts
// cv-qualified integral types and preserves their qualifiers. This keeps its
// domain aligned with integral_like's cv-transparent built-in branch.
template<class T>
struct make_unsigned_like {};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_unsigned_like<T> : std::make_unsigned<T> {};

// The not-integral term is what keeps this disjoint from the specialization
// above rather than more or less specialized than it: a built-in unsigned type
// is answered there, by std::make_unsigned, and only the class types reach
// here. is_unsigned_like_v is total, so asking it of an arbitrary T is safe,
// and it already implies is_integral_like_v for a non-integral type - no
// built-in floating-point type is unsigned, and no class type is
// arithmetic-like without being integer-like. The integer-like term is spelled
// out anyway, because the domain this trait means to cover is the integer-like
// types and a reader should not have to derive that.
template<class T>
        requires (not std::is_integral_v<T>) and is_integral_like_v<T> and is_unsigned_like_v<T>
struct make_unsigned_like<T> : std::type_identity<T> {};

template<class T>
using make_unsigned_like_t = make_unsigned_like<T>::type;

// __int128 is where "integral" stops being a property of a type and becomes a
// property of the dialect. GCC and Clang predefine __GLIBCXX_TYPE_INT_N_0
// only outside __STRICT_ANSI__, and libstdc++ gates both its std::is_integral
// entry and its __make_unsigned overload on that macro, so in the strictly
// conforming dialect this library targets the partial specialization above
// does not match and std::make_unsigned<__int128> is a hard error rather than
// a substitution failure. libc++ has no such gate, so there the partial
// specialization matches in every dialect and this explicit specialization
// merely agrees with it. GCC withholds the integral answer because intmax_t
// has to be the widest signed integer type and is 64-bit for ABI reasons - a
// contradiction it resolves in the standard's favor exactly when asked to be
// standard, and lets stand otherwise, where std::signed_integral<__int128>
// and sizeof(std::intmax_t) == 8 both hold at once.
//
// The type is a language extension, so naming it is what -Wpedantic is for.
// Suppressing that diagnostic is the whole service this specialization
// performs: without it every user of a 128-bit division would have to write
// the specialization, and the suppression, themselves. The pragma is guarded
// because MSVC, which has no __int128 to have an opinion about, does not know
// it.
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
template<>
struct make_signed_like<unsigned __int128> : std::type_identity<__int128> {};

template<>
struct make_unsigned_like<__int128> : std::type_identity<unsigned __int128> {};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif

#if defined(_MSVC_STL_VERSION) && !defined(__SIZEOF_INT128__)
template<>
struct make_signed_like<uint128_t> : std::type_identity<int128_t> {};

template<>
struct make_unsigned_like<int128_t> : std::type_identity<uint128_t> {};
#endif

template<class Tag>
struct empty_type
{
        [[nodiscard]] constexpr empty_type() noexcept = default;

        // constrained so that this catch-all never hijacks copy or move
        // construction from the (trivial) special member functions
        // clang-format off
        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept {}
        // clang-format on

        // a hidden friend, so it is found by argument-dependent lookup only;
        // still implicitly declares the defaulted operator== that lets an
        // enclosing class default its own comparisons over this member
        [[nodiscard]] friend constexpr auto operator<=>(empty_type, empty_type) noexcept -> std::strong_ordering = default;
};

// Tag names the member, not the type it stands in for: two
// [[no_unique_address]] subobjects of the same empty type must have
// distinct addresses, so two absent members sharing a tag stop overlapping
// and grow the class. Tagging by Type instead would make that collision a
// silent function of the member types, so Tag is required rather than
// defaulted. An elaborated-type-specifier declares one in place, without a
// separate declaration per member:
//
//      conditional_data_member_t<Condition, Type, struct variable_tag> m_variable [[no_unique_address]];
//
template<bool Condition, class Type, class Tag>
using conditional_data_member_t = std::conditional_t<Condition, Type, empty_type<Tag>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_HPP
