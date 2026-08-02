//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

#include <compare>     // strong_ordering (empty_type's defaulted <=>)
#include <concepts>    // constructible_from, totally_ordered
#include <limits>      // numeric_limits
#include <type_traits> // bool_constant, conditional_t, integral_constant, is_abstract_v, is_arithmetic_v, is_array_v, is_integral_v, is_object_v, is_same_v, make_unsigned, remove_cv_t, remove_cvref_t

namespace xstd {

// std::is_arithmetic, opened to class types - and the trait the other two
// below, and xstd::integral_like, are built on.
//
// std::is_arithmetic_v is the root of the closed list: is_integral_v and
// is_floating_point_v feed it, is_signed_v and is_unsigned_v are spelled over
// it, and none of them can ever be true for a class type. Opening the root is
// what lets the rest follow, rather than each one needing its own carve-out.
//
// std::numeric_limits is the marker for "arithmetic" here, because it is the
// one the standard already uses that way: it is specialized for exactly the
// arithmetic types, and a class type that means to behave like a number
// specializes it too - the standard's own integer-class types
// ([iterator.concept.winc]) and every extended-precision library do.
//
// The requires-clause is not a filter on what counts as arithmetic; it is what
// makes the question askable at all. std::numeric_limits' primary template
// declares static member functions returning T, which is ill-formed rather
// than merely unspecialized for an array type or an abstract class, so those
// have to be answered by the unconstrained primary below - which reports
// std::is_arithmetic_v's own answer, false - before numeric_limits is ever
// named. A concept could lean on conjunction short-circuiting instead; a
// variable template's initializer has to be well-formed in every operand, so
// the guard moves into the constraint, where a requires-clause's conjunction
// short-circuits in the same way.
//
// That short-circuiting is load-bearing in the middle of the clause too: the
// sizeof test has to be passed before std::is_abstract_v is named, because
// is_abstract_v requires a complete type while std::is_arithmetic_v answers
// false for an incomplete one quite happily. Without it this trait would be a
// hard error exactly where the trait it widens is merely false, which is not
// a widening.
template<class T>
inline constexpr auto is_arithmetic_like_v = std::is_arithmetic_v<T>;

template<class T>
        requires (not std::is_arithmetic_v<T>) and std::is_object_v<T> and (not std::is_array_v<T>) and requires { sizeof(T); } and (not std::is_abstract_v<T>)
inline constexpr auto is_arithmetic_like_v<T> = std::numeric_limits<T>::is_specialized;

template<class T>
using is_arithmetic_like = std::bool_constant<is_arithmetic_like_v<T>>;

// std::is_signed and std::is_unsigned, opened the same way, and spelled the
// same way the standard spells them - is_arithmetic_v<T> && T(-1) < T(0) -
// with the opened arithmetic test in place of the closed one. Asking the type
// rather than reading std::numeric_limits<T>::is_signed keeps these two
// definitionally identical to the standard's, so a built-in answer can never
// drift from std::is_signed_v.
//
// The two extra constraints are again about askability rather than meaning:
// forming T(-1) needs a T constructible from int, and comparing needs an
// ordering. An arithmetic-like type without both cannot be asked the question,
// and falls to the primary's false.
//
// One note for authors of arithmetic-like class types: static_cast<T>(-1) is a
// constructor call, so a type whose only viable constructor takes an *unsigned*
// parameter converts the -1 implicitly and draws -Wsign-conversion here, at
// this line, inside this header. Giving the type a constructor from a signed
// integral type - which [iterator.concept.winc] asks of integer-class types
// anyway - is what avoids it. There is deliberately no pragma silencing it
// from xstd's side: the conversion really is happening in the user's type, and
// the diagnostic is pointing at something worth fixing there.
template<class T>
inline constexpr auto is_signed_like_v = false;

template<class T>
        requires is_arithmetic_like_v<T> and std::constructible_from<T, int> and std::totally_ordered<T>
// Both checks fire only on the bool instantiation, where -1 and 0 are the
// standard's own spelling of this test and turning them into bool literals
// would change what is being asked. T is a template parameter, so neither
// diagnostic is about the code as written.
// NOLINTNEXTLINE(readability-implicit-bool-conversion,modernize-use-bool-literals)
inline constexpr auto is_signed_like_v<T> = static_cast<T>(-1) < static_cast<T>(0);

template<class T>
using is_signed_like = std::bool_constant<is_signed_like_v<T>>;

// No guard needed: both operands are well-formed for every T, and the pair
// partitions the arithmetic-like types exactly as std::is_signed_v and
// std::is_unsigned_v partition the arithmetic ones. bool lands on the unsigned
// side of that partition in both spellings.
template<class T>
inline constexpr auto is_unsigned_like_v = is_arithmetic_like_v<T> and not is_signed_like_v<T>;

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

// The unsigned type that pairs with a signed integer-like type: what
// std::make_unsigned answers, for the types it is willing to answer for.
//
// std::make_unsigned is closed in two separate ways. Its domain is the
// built-in integral types other than bool, plus the enumerations - a list no
// class type can join. And outside that domain it is ill-formed rather than
// empty, so naming it on an unconstrained template parameter is a hard error
// instead of a substitution failure, which is precisely what a concept cannot
// survive: the check that was supposed to answer "no" stops the compile
// instead. Hence the deliberately empty primary template here.
// make_unsigned_like_t<T> is then a substitution failure - detectable, not
// fatal - for every type that has no unsigned counterpart, and a user can
// give one to a type of their own by specializing it. xstd::signed_integral_like
// is built on exactly that, and is the reason this trait exists.
//
// Unlike std::make_unsigned this says nothing about cv-qualified types. A
// cv-qualified type is not std::regular (it is not assignable), so it is
// never xstd::integral_like either, and there is no signed integer-like type
// whose counterpart a cv-qualified answer would name. Leaving them out is
// also what keeps the trait's domain the same on every platform: the
// __int128 specialization below names one type, and a cv-carrying trait
// would answer for its qualified forms only where the partial specialization
// happens to match them.
//
// std::is_integral_v is true for cv-qualified types as well, so the
// remove_cv_t test is what does that restricting - it is not redundant.
template<class T>
struct make_unsigned_like
{};

template<class T>
        requires std::is_same_v<T, std::remove_cv_t<T>> and std::is_integral_v<T> and (not std::is_same_v<T, bool>)
struct make_unsigned_like<T> : std::make_unsigned<T>
{};

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
struct make_unsigned_like<__int128>
{
        using type = unsigned __int128;
};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
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
        [[nodiscard]] friend constexpr auto operator<=>(empty_type const&, empty_type const&) noexcept
                -> std::strong_ordering = default;
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
