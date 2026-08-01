//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDLIB_HPP
#define XSTD_CSTDLIB_HPP

#include <cassert>     // assert
#include <concepts>    // signed_integral
#include <cstdint>     // intmax_t
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
[[nodiscard]] constexpr T abs(T x) noexcept
{
        assert(x != std::numeric_limits<T>::min()); // -x would overflow
        return x < 0 ? static_cast<T>(-x) : x;
}

// The total counterpart of abs: same |x|, but returning the unsigned type, so
// the one input abs has to exclude - the most negative value, whose magnitude
// is one past the signed maximum - is in contract here. Negation is done by
// unsigned wraparound, which is well-defined, rather than by -x on a signed
// MIN, which is not, or by widening to a bigger signed type, which has none
// to widen to at the widest end.
template<std::signed_integral T>
[[nodiscard]] constexpr std::make_unsigned_t<T> uabs(T x) noexcept
{
        using U = std::make_unsigned_t<T>;
        auto const u = static_cast<U>(x);
        // The cast back to U is what makes this wraparound rather than
        // promotion: for types narrower than int, U{0} - u is evaluated in
        // int and is negative, and converting that back to U reduces it mod
        // 2^N - exactly the value the wider unsigned types get directly.
        return x < 0 ? static_cast<U>(U{0} - u) : u;
}

// not part of <cstdlib>, but kept to the same shape as abs/uabs above. The
// result is a plain int at every width: a sign is a three-valued quantity,
// not a number in T's range.
template<std::signed_integral T>
[[nodiscard]] constexpr int sign(T x) noexcept
{
        return static_cast<int>(0 < x) - static_cast<int>(x < 0);
}

template<std::signed_integral T>
struct div_t
{
        T quot, rem;
        [[nodiscard]] friend constexpr bool operator==(div_t const&, div_t const&) noexcept = default;
};

// Aggregate class template argument deduction would already deduce this, but
// spelling the guide out makes the support intentional rather than incidental
// (and keeps -Wctad-maybe-unsupported quiet), so div_t{q, r} remains as
// writable as the four separate div_t/ldiv_t/lldiv_t/imaxdiv_t names were.
template<std::signed_integral T>
div_t(T, T) -> div_t<T>;

namespace detail {

// Whether the back-multiplication self-check numer == denom * q + r can be
// evaluated without the product overflowing depends on which q it is.
//
// For truncated division it always can, in T itself: denom * qT is exactly
// numer - rT, and rT carries numer's sign, so the product lies between 0 and
// numer inclusive and is representable wherever numer is. No widening, at
// any width.
//
// The adjusted quotients are the ones that need room. euclidean_div and
// floored_div move the remainder across zero, so denom * q = numer - r can
// land one unit of |denom| outside T - int32_t's numer == INT32_MIN with
// denom == 3 gives denom * qE == INT32_MIN - 1. The excess is bounded by
// |numer| + |denom| < 2^N for an N-bit T, so one extra bit is enough and any
// wider signed type will do; intmax_t is the widest portably available. For a
// T that is already intmax_t-wide there is nothing to widen to, and the sign
// and magnitude assertions have to carry the contract on their own - the same
// tradeoff the non-template ldiv/lldiv/imaxdiv made, now stated once and
// applied only where it actually bites.
template<std::signed_integral T>
inline constexpr auto has_wider_type = sizeof(T) < sizeof(std::intmax_t);

} // namespace detail

// C++ Standard [expr.mul]/4
// https://en.wikipedia.org/wiki/Modulo_operation
// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

// constexpr version of <cstdlib>'s div/ldiv/lldiv and <cinttypes>'s imaxdiv.
// %: C99, C++11, C#, D, F#, Go, Java, Javascript, PHP, Rust, Scala, Swift
// rem: Ada, Clojure, Erlang, Haskell, Julia, Lisp, Prolog
// remainder: Ruby, Scheme
// mod: Fortran, OCaml
template<std::signed_integral T>
[[nodiscard]] constexpr div_t<T> div(T numer, T denom) noexcept
{
        assert(denom != 0);
        assert(!(numer == std::numeric_limits<T>::min() && denom == -1));
        auto const qT = static_cast<T>(numer / denom);
        auto const rT = static_cast<T>(numer % denom);
        assert(numer == (denom * qT) + rT); // see detail::has_wider_type: safe in T at every width
        assert(uabs(rT) < uabs(denom));
        assert(sign(rT) == sign(numer) || rT == 0);
        return {.quot = qT, .rem = rT};
}

// https://en.wikipedia.org/wiki/Euclidean_division
// mod: Maple, Pascal
// modulo: Scheme
template<std::signed_integral T>
[[nodiscard]] constexpr div_t<T> euclidean_div(T numer, T denom) noexcept
{
        assert(denom != 0);
        auto const divT = div(numer, denom);
        auto const I = divT.rem >= 0 ? T{0} : (denom > 0 ? T{1} : T{-1});
        auto const qE = static_cast<T>(divT.quot - I);
        auto const rE = static_cast<T>(divT.rem + (I * denom));
        if constexpr (detail::has_wider_type<T>) {
                assert(static_cast<std::intmax_t>(numer) == (static_cast<std::intmax_t>(denom) * qE) + rE);
        }
        assert(uabs(rE) < uabs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

// %: Perl, Python, Ruby
// %%: R
// mod: Ada, Clojure, Haskell, Julia, Lisp, ML, Prolog
// modulo: Fortran, Ruby
template<std::signed_integral T>
[[nodiscard]] constexpr div_t<T> floored_div(T numer, T denom) noexcept
{
        assert(denom != 0);
        auto const divT = div(numer, denom);
        auto const I = sign(divT.rem) == -sign(denom) ? T{1} : T{0};
        auto const qF = static_cast<T>(divT.quot - I);
        auto const rF = static_cast<T>(divT.rem + (I * denom));
        if constexpr (detail::has_wider_type<T>) {
                assert(static_cast<std::intmax_t>(numer) == (static_cast<std::intmax_t>(denom) * qF) + rF);
        }
        assert(uabs(rF) < uabs(denom));
        assert(rF == 0 || sign(rF) == sign(denom));
        return {.quot = qF, .rem = rF};
}

} // namespace xstd

// Specialized via qualified-id (template<class T> struct std::formatter<...>)
// rather than inside a reopened "namespace std { ... }" block: both forms
// are equally legal here (the standard explicitly permits specializing
// std::formatter for program-defined types), but the qualified form avoids
// clang-tidy's bugprone-std-namespace-modification finding, which otherwise
// flags any reopening of namespace std regardless of what's inside it.
//
// A partial specialization over div_t's element type also defers the body's
// instantiation to the point of use, so merely including this header no
// longer requires a standard library that implements tuple formatting; only
// actually formatting an xstd::div_t does.
template<class T>
struct std::formatter<xstd::div_t<T>> : std::formatter<std::tuple<T const&, T const&>>
{
        auto format(xstd::div_t<T> const& d, auto& ctx) const
        {
                return std::formatter<std::tuple<T const&, T const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

namespace xstd {

// narrow std::ostream only, not the full basic_ostream<charT, traits>
// generality (no wide-character support): this exists solely so Boost.Test
// can print div_t values in test diagnostics. Application code should format
// these types via std::format/std::print directly rather than through
// operator<<.
template<std::signed_integral T>
auto& operator<<(std::ostream& ostr, div_t<T> const& d)
{
        return ostr << std::format("{}", d);
}

} // namespace xstd

#endif // XSTD_CSTDLIB_HPP
