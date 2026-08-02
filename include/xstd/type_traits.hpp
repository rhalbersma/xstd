//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

#include <compare>     // strong_ordering (empty_type's defaulted <=>)
#include <type_traits> // bool_constant, conditional_t, integral_constant, is_integral_v, is_same_v, make_unsigned, remove_cv_t, remove_cvref_t

namespace xstd {

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
