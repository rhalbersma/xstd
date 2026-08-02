//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_HPP
#define XSTD_CSTDLIB_HPP

#include <cassert>     // assert
#include <concepts>    // signed_integral
#include <format>      // format, formatter
#include <limits>      // numeric_limits
#include <ostream>     // ostream
#include <tuple>       // tie, tuple
#include <type_traits> // make_unsigned_t

namespace xstd {

// Each facility below is a single function template constrained to
// std::signed_integral, rather than the <cstdlib>-style family of four
// fixed-width overloads (abs/labs/llabs/imaxabs and friends) these grew out
// of. Consequences worth knowing at the call site:
//
// - The exact-width types are covered for free. int8_t/int16_t/int32_t/
//   int64_t are aliases of signed char/short/int/long/long long, so a
//   template over std::signed_integral instantiates for each of them,
//   including the two narrow widths the <cstdlib> naming has no name for.
// - The result type is the argument type, not the promoted type. Passing an
//   int16_t no longer silently widens to int the way a call to a non-template
//   abs(int) did; abs(x) of an int16_t is an int16_t, and its precondition is
//   int16_t's, not int's. Callers who want the old promoting behavior can
//   write abs(+x) or abs<int>(x).
// - The two-argument templates deduce a single T from both arguments, so a
//   mixed-width call like div(8, 3L) is a deduction failure rather than a
//   silent conversion. Spell the intent as div<long>(8, 3L) or cast.
// - 128-bit integers are deliberately not covered: neither __int128 nor
//   unsigned __int128 satisfies std::integral in the strictly conforming
//   dialect this library targets, and libstdc++'s std::make_unsigned is a
//   hard error rather than a substitution failure for them. Widening the
//   constraint is a one-line change if that ever becomes portable; see
//   doc/design.md.

// constexpr version of <cstdlib>'s abs/labs/llabs and <cinttypes>'s imaxabs
// (P0533), generalized to one signed-only template.
template<std::signed_integral T>
[[nodiscard]] constexpr auto abs(T x) noexcept
        -> T
{
        assert(x != std::numeric_limits<T>::min()); // -x would overflow
        return static_cast<T>(x < 0 ? -x : x);
}

// The total counterpart of abs: same |x|, but returning the unsigned type, so
// the one input abs has to exclude - the most negative value, whose magnitude
// is one past the signed maximum - is in contract here. Negation is done by
// unsigned wraparound, which is well-defined, rather than by -x on a signed
// MIN, which is not, or by widening to a bigger signed type, which has none
// to widen to at the widest end.
//
// The return type is deduced rather than spelled std::make_unsigned_t<T>,
// which is the one signature in this header where that choice is load-bearing
// rather than cosmetic. Clang before 21 does not implement CWG2369: it
// substitutes the deduced arguments into the function type before checking the
// constraint, so a spelled-out return type instantiates make_unsigned_t<double>
// for uabs(1.0) - a hard error rather than a substitution failure, which makes
// the call ill-formed inside a requires-expression instead of merely false.
// Deducing keeps the trait out of the signature, so std::signed_integral gets
// to reject the argument first. Every other return type here (T, int, bool,
// div_t<T>) instantiates no trait and is spelled out. See doc/design.md.
template<std::signed_integral T>
[[nodiscard]] constexpr auto uabs(T x) noexcept
{
        using U = std::make_unsigned_t<T>;
        auto const u = static_cast<U>(x);
        // The cast back to U is what makes this wraparound rather than
        // promotion: for types narrower than int, 0 - u is evaluated in int
        // and is negative, and converting that back to U reduces it mod 2^N -
        // exactly the value the wider unsigned types get directly. The zero
        // needs no type of its own: the usual arithmetic conversions give the
        // subtraction the same type either way.
        return static_cast<U>(x < 0 ? 0 - u : u);
}

// not part of <cstdlib>, but kept to the same shape as abs/uabs above. The
// result is a plain int at every width: a sign is a three-valued quantity,
// not a number in T's range.
template<std::signed_integral T>
[[nodiscard]] constexpr auto sign(T x) noexcept
        -> int
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

template<std::signed_integral T>
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
template<std::signed_integral T>
div_t(T, T) -> div_t<T>;

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// constexpr version of <cstdlib>'s div/ldiv/lldiv and <cinttypes>'s imaxdiv.
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
template<std::signed_integral T>
[[nodiscard]] constexpr auto div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != 0);
        assert(numer != std::numeric_limits<T>::min() or denom != -1);
        auto const qT = static_cast<T>(numer / denom);
        auto const rT = static_cast<T>(numer % denom);
        // Safe in T at every width, with no widening: denom * qT is exactly
        // numer - rT, and rT carries numer's sign, so the product lies between
        // 0 and numer inclusive and is representable wherever numer is. This
        // is the only one of the three conventions where the identity is worth
        // asserting - it checks the built-in / and % against each other. For
        // the two below it is an algebraic consequence of how they adjust qT
        // and rT, so it could only fail if this one had already failed.
        assert(numer == (denom * qT) + rT);
        assert(uabs(rT) < uabs(denom));
        assert(sign(rT) == sign(numer) or rT == 0);
        return {.quot = qT, .rem = rT};
}

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
template<std::signed_integral T>
[[nodiscard]] constexpr auto euclidean_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != 0);
        auto const [qT, rT] = div(numer, denom);
        // A negative truncated remainder is moved up by one |denom|, with the
        // quotient compensated in the matching direction. Both lines select a
        // whole expression rather than adding a delta, because the delta here
        // is |denom|: denom == MIN is in contract and -MIN is not
        // representable, so the value can never be formed on its own. Spelling
        // it as an add or a subtract of denom chosen by denom's sign keeps
        // every intermediate in range. (floored_div below adjusts by denom
        // itself, which is always representable, so it can add a delta.)
        auto const adjust = rT < 0;
        auto const qE = static_cast<T>(adjust ? (denom > 0 ? qT - 1 : qT + 1) : qT);
        auto const rE = static_cast<T>(adjust ? (denom > 0 ? rT + denom : rT - denom) : rT);
        assert(uabs(rE) < uabs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
template<std::signed_integral T>
[[nodiscard]] constexpr auto floored_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != 0);
        auto const [qT, rT] = div(numer, denom);
        // The same adjustment as euclidean_div above, but two-valued rather
        // than three: a remainder whose sign differs from denom's is moved one
        // denom in denom's direction, and the quotient compensated. The delta
        // is denom itself rather than |denom|, and denom is by definition
        // representable, so both lines can add a conditional delta instead of
        // selecting a whole expression. Neither zero needs a type of its own.
        auto const adjust = sign(rT) == -sign(denom);
        auto const qF = static_cast<T>(qT - (adjust ? 1 : 0));
        auto const rF = static_cast<T>(rT + (adjust ? denom : 0));
        assert(uabs(rF) < uabs(denom));
        assert(rF == 0 or sign(rF) == sign(denom));
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
