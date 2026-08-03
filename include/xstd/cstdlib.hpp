//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_HPP
#define XSTD_CSTDLIB_HPP

#include <xstd/concepts.hpp>    // signed_integral_like
#include <xstd/type_traits.hpp> // make_unsigned_like_t
#include <cassert>              // assert
#include <format>               // format, formatter
#include <limits>               // numeric_limits
#include <ostream>              // ostream
#include <tuple>                // tie, tuple

namespace xstd {

// Each facility below is a single function template constrained to
// xstd::signed_integral_like, rather than the <cstdlib>-style family of four
// fixed-width overloads (abs/labs/llabs/imaxabs and friends) these grew out
// of. Consequences worth knowing at the call site:
//
// - The exact-width types are covered for free. int8_t/int16_t/int32_t/
//   int64_t are aliases of signed char/short/int/long/long long, so one
//   template instantiates for each of them, including the two narrow widths
//   the <cstdlib> naming has no name for.
// - The result type is the argument type, not the promoted type. Passing an
//   int16_t no longer silently widens to int the way a call to a non-template
//   abs(int) did; abs(x) of an int16_t is an int16_t, and its precondition is
//   int16_t's, not int's. Callers who want the old promoting behavior can
//   write abs(+x) or abs<int>(x).
// - The two-argument templates deduce a single T from both arguments, so a
//   mixed-width call like div(8, 3L) is a deduction failure rather than a
//   silent conversion. Spell the intent as div<long>(8, 3L) or cast.
// - 128-bit integers are covered, on both of the shapes they come in: the
//   built-in __int128, whose std::is_integral answer depends on the dialect,
//   and the integer *class* types (libstdc++'s __max_diff_type, the MSVC
//   STL's _Signed128) that no dialect could ever make integral. That is what
//   xstd::signed_integral_like buys over std::signed_integral; see
//   <xstd/concepts.hpp> and doc/design.md.
//
// Every literal below is spelled static_cast<T>(...) rather than written
// bare. An integer-class type is only required to be explicitly convertible
// from an integral type, so "denom != 0" need not compile for one while
// "denom != static_cast<T>(0)" does. For the built-in widths the two are the
// same expression after promotion.

// not part of <cstdlib>, but kept to the same shape as everything below it.
// The result is a plain int at every width: a sign is a three-valued quantity,
// not a number in T's range.
template<signed_integral_like T>
[[nodiscard]] constexpr auto sign(T x) noexcept
        -> int
{
        auto const zero = static_cast<T>(0);
        return static_cast<int>(zero < x) - static_cast<int>(x < zero);
}

// constexpr version of <cstdlib>'s abs/labs/llabs and <cinttypes>'s imaxabs
// (P0533), generalized to one signed-only template.
template<signed_integral_like T>
[[nodiscard]] constexpr auto abs(T x) noexcept
        -> T
{
        auto const zero = static_cast<T>(0);
        assert(x != std::numeric_limits<T>::min()); // -x would overflow
        return static_cast<T>(x < zero ? -x : x);
}

// The total counterpart of abs: same |x|, but returning the unsigned type, so
// the one input abs has to exclude - the most negative value, whose magnitude
// is one past the signed maximum - is in contract here. Negation is done by
// unsigned wraparound, which is well-defined, rather than by -x on a signed
// MIN, which is not, or by widening to a bigger signed type, which has none
// to widen to at the widest end.
//
// The return type is deduced rather than spelled make_unsigned_like_t<T>,
// which is the one signature in this header where that choice is load-bearing
// rather than cosmetic. Clang before 21 does not implement CWG2369: it
// substitutes the deduced arguments into the function type before checking the
// constraint, so a spelled-out return type instantiates the trait for
// uabs(1.0) even though the constraint would have rejected the argument.
// xstd::make_unsigned_like is empty rather than ill-formed for a double, so
// this would now be a substitution failure rather than the hard error
// std::make_unsigned_t<double> used to be - but a substitution failure in the
// return type is still a worse diagnostic than a failed constraint, and one
// that reports the wrong reason. Deducing keeps the trait out of the
// signature, so signed_integral_like gets to reject the argument first. Every
// other return type here (T, int, bool, div_t<T>) instantiates no trait and
// is spelled out. See doc/design.md.
template<signed_integral_like T>
[[nodiscard]] constexpr auto uabs(T x) noexcept
{
        using U = make_unsigned_like_t<T>;
        auto const zero = static_cast<U>(0);
        auto const u = static_cast<U>(x);
        // The cast back to U is what makes this wraparound rather than
        // promotion: for types narrower than int, zero - u is evaluated in int
        // and is negative, and converting that back to U reduces it mod 2^N -
        // exactly the value the wider unsigned types get directly.
        return static_cast<U>(x < static_cast<T>(0) ? zero - u : u);
}

template<signed_integral_like T>
struct div_t
{
        T quot, rem;
        [[nodiscard]] friend constexpr auto operator==(div_t const&, div_t const&) noexcept -> bool = default;

        // narrow std::ostream only, not the full basic_ostream<charT, traits>
        // generality (no wide-character support): this exists solely so
        // Boost.Test can print div_t values in test diagnostics. Application
        // code should format these types via std::format/std::print directly
        // rather than through operator<<.
        //
        // A hidden friend, matching operator== above: reachable by
        // argument-dependent lookup on div_t and nothing else, so it never
        // joins the candidate set of an unrelated operator<<. Deliberately
        // not [[nodiscard]] - discarding the returned stream is what an
        // ordinary "ostr << d;" statement does. The body names the formatter
        // specialization declared below this class, which is fine: a friend
        // definition is only instantiated at its point of use.
        friend auto operator<<(std::ostream& ostr, div_t const& d)
                -> std::ostream&
        {
                return ostr << std::format("{}", d);
        }
};

// Aggregate class template argument deduction would already deduce this, but
// spelling the guide out makes the support intentional rather than incidental
// (and keeps -Wctad-maybe-unsupported quiet), so div_t{q, r} remains as
// writable as the four separate div_t/ldiv_t/lldiv_t/imaxdiv_t names were.
template<signed_integral_like T>
div_t(T, T) -> div_t<T>;

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// constexpr version of <cstdlib>'s div/ldiv/lldiv and <cinttypes>'s imaxdiv.
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
template<signed_integral_like T>
[[nodiscard]] constexpr auto div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        assert(numer != std::numeric_limits<T>::min() or denom != static_cast<T>(-1));
        auto const qT = static_cast<T>(numer / denom);
        auto const rT = static_cast<T>(numer % denom);
        // Safe in T at every width, with no widening: denom * qT is exactly
        // numer - rT, and rT carries numer's sign, so the product lies between
        // 0 and numer inclusive and is representable wherever numer is. This
        // is the only one of the three conventions where the identity is worth
        // asserting - it checks / and % against each other. For the two below
        // it is an algebraic consequence of how they adjust qT and rT, so it
        // could only fail if this one had already failed.
        assert(numer == static_cast<T>(static_cast<T>(denom * qT) + rT));
        assert(uabs(rT) < uabs(denom));
        assert(sign(rT) == sign(numer) or rT == static_cast<T>(0));
        return {.quot = qT, .rem = rT};
}

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
template<signed_integral_like T>
[[nodiscard]] constexpr auto euclidean_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<T>(0);
        auto const one = static_cast<T>(1);
        // A negative truncated remainder is moved up by one |denom|, with the
        // quotient compensated in the matching direction. Both lines select a
        // whole expression rather than adding a delta, because the delta here
        // is |denom|: denom == MIN is in contract and -MIN is not
        // representable, so the value can never be formed on its own. Spelling
        // it as an add or a subtract of denom chosen by denom's sign keeps
        // every intermediate in range. (floored_div below adjusts by denom
        // itself, which is always representable, so it can add a delta.)
        //
        // Each arm carries its own static_cast<T> rather than one cast around
        // the whole conditional. For the built-in widths that is the same
        // narrowing in a different place, but the operands of a conditional
        // have to agree on a type before it can be cast at all, and qT - one
        // is a T only after the cast - a class type has no integer promotions
        // to fall back on the way int8_t does.
        auto const adjust = rT < zero;
        auto const qE = adjust ? (denom > zero ? static_cast<T>(qT - one) : static_cast<T>(qT + one)) : qT;
        auto const rE = adjust ? (denom > zero ? static_cast<T>(rT + denom) : static_cast<T>(rT - denom)) : rT;
        assert(uabs(rE) < uabs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
template<signed_integral_like T>
[[nodiscard]] constexpr auto floored_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<T>(0);
        auto const one = static_cast<T>(1);
        // The same adjustment as euclidean_div above, but two-valued rather
        // than three: a remainder whose sign differs from denom's is moved one
        // denom in denom's direction, and the quotient compensated. The delta
        // is denom itself rather than |denom|, and denom is by definition
        // representable, so both lines add a conditional delta instead of
        // selecting a whole expression - the delta, not the result, is what
        // the conditional picks.
        auto const adjust = sign(rT) == -sign(denom);
        auto const qF = static_cast<T>(qT - (adjust ? one : zero));
        auto const rF = static_cast<T>(rT + (adjust ? denom : zero));
        assert(uabs(rF) < uabs(denom));
        assert(rF == static_cast<T>(0) or sign(rF) == sign(denom));
        return {.quot = qF, .rem = rF};
}

} // namespace xstd

// Specialized via qualified-id (template<class T> struct std::formatter<...>)
// rather than inside a reopened "namespace std { ... }" block. Both forms are
// equally legal here - [namespace.std]/2 explicitly permits adding a template
// specialization to namespace std when it depends on a program-defined type
// and meets the original template's requirements - and the qualified form is
// the narrower of the two, since it can only ever declare the one
// specialization it names.
//
// clang-tidy's bugprone-std-namespace-modification used to flag only the
// reopened form, which is what this spelling was chosen for; as of clang-tidy
// 22 it flags both, without exempting the specializations the standard allows.
// Silenced on the declaration rather than repo-wide: the check still has a
// real job to do on any other addition to namespace std.
//
// A partial specialization over div_t's element type also defers the body's
// instantiation to the point of use, so merely including this header no
// longer requires a standard library that implements tuple formatting; only
// actually formatting an xstd::div_t does.
//
// Formatting is the one xstd operation that is not usable at compile time,
// and not by its own choice: the tuple formatter it delegates to is not
// constexpr in C++23. Marking format() constexpr anyway would be ill-formed,
// no diagnostic required - no specialization of it could ever be evaluated
// in a constant expression, and [dcl.constexpr]/6 says as much about a
// templated function no specialization of which could be constexpr - so both
// compilers would accept it in silence rather than reject it.
//
// P3391 (constexpr std::format), plenary-approved for C++29, makes the
// standard library's own formatter specializations constexpr, the tuple one
// included, and announces that with __cpp_lib_constexpr_format. Inheriting
// from a constexpr-enabled base is not enough on its own: a format call is a
// constant expression only if every formatter specialization it uses is
// constexpr-enabled, and this one is a specialization of its own. The macro
// below is that opt-in, spelled conditionally so the keyword appears exactly
// where the delegate can honor it and nowhere else. It is #undef'd right
// after its single use: it is a detail of this one member function rather
// than part of xstd's interface, and macro replacement happens long before
// the member is instantiated, so nothing downstream needs it to stay
// defined. Floating-point, locale-aware and chrono formatting stay outside
// P3391; none of the three is reachable from a div_t<T> of signed integers.
#ifdef __cpp_lib_constexpr_format
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

template<class T>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_t<T>> : std::formatter<std::tuple<T const&, T const&>>
{
        // constexpr only where the inherited tuple formatter is; see the note
        // above. parse() needs no such gate and is not overridden here: the
        // standard formatters' parse has had to be usable in a constant
        // expression since C++20 - that is what makes compile-time checking
        // of the format string work - so the inherited one is constexpr
        // already, P3391 or not.
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<T> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                return std::formatter<std::tuple<T const&, T const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_CSTDLIB_HPP
