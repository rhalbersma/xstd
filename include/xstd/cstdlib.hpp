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

// Signed-integer algorithms generalized from <cstdlib> to integral-like types.

// Returns -1, 0, or 1.
template<signed_integral_like T>
[[nodiscard]] constexpr auto sign(T x) noexcept
        -> int
{
        auto const zero = static_cast<T>(0);
        return static_cast<int>(zero < x) - static_cast<int>(x < zero);
}

// constexpr generalization of abs/labs/llabs/imaxabs.
template<signed_integral_like T>
[[nodiscard]] constexpr auto abs(T x) noexcept
        -> T
{
        auto const zero = static_cast<T>(0);
        assert(x != std::numeric_limits<T>::min()); // -x would overflow
        return static_cast<T>(x < zero ? -x : x);
}

// Total absolute value, returning the unsigned counterpart so MIN is valid.
template<signed_integral_like T>
[[nodiscard]] constexpr auto uabs(T x) noexcept
{
        using U = make_unsigned_like_t<T>;
        auto const zero = static_cast<U>(0);
        auto const u = static_cast<U>(x);
        return static_cast<U>(x < static_cast<T>(0) ? zero - u : u);
}

template<signed_integral_like T>
struct div_t
{
        T quot, rem;
        [[nodiscard]] friend constexpr auto operator==(div_t const&, div_t const&) noexcept -> bool = default;

        // Narrow stream support for test diagnostics.
        friend auto operator<<(std::ostream& ostr, div_t const& d)
                -> std::ostream&
        {
                return ostr << std::format("{}", d);
        }
};

// Explicit to keep -Wctad-maybe-unsupported quiet.
template<signed_integral_like T>
div_t(T, T) -> div_t<T>;

// Truncated division, as specified by [expr.mul].
template<signed_integral_like T>
[[nodiscard]] constexpr auto div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        assert(numer != std::numeric_limits<T>::min() or denom != static_cast<T>(-1));
        auto const qT = static_cast<T>(numer / denom);
        auto const rT = static_cast<T>(numer % denom);
        assert(numer == static_cast<T>(static_cast<T>(denom * qT) + rT));
        assert(uabs(rT) < uabs(denom));
        assert(sign(rT) == sign(numer) or rT == static_cast<T>(0));
        return {.quot = qT, .rem = rT};
}

// Euclidean division: the remainder is nonnegative.
template<signed_integral_like T>
[[nodiscard]] constexpr auto euclidean_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<T>(0);
        auto const one = static_cast<T>(1);
        auto const adjust = rT < zero;
        auto const qE = adjust ? (denom > zero ? static_cast<T>(qT - one) : static_cast<T>(qT + one)) : qT;
        auto const rE = adjust ? (denom > zero ? static_cast<T>(rT + denom) : static_cast<T>(rT - denom)) : rT;
        assert(uabs(rE) < uabs(denom));
        assert(sign(rE) >= 0);
        return {.quot = qE, .rem = rE};
}

// Floored division: the remainder has the denominator's sign.
template<signed_integral_like T>
[[nodiscard]] constexpr auto floored_div(T numer, T denom) noexcept
        -> div_t<T>
{
        assert(denom != static_cast<T>(0));
        auto const [qT, rT] = div(numer, denom);
        auto const zero = static_cast<T>(0);
        auto const one = static_cast<T>(1);
        auto const adjust = sign(rT) == -sign(denom);
        auto const qF = static_cast<T>(qT - (adjust ? one : zero));
        auto const rF = static_cast<T>(rT + (adjust ? denom : zero));
        assert(uabs(rF) < uabs(denom));
        assert(rF == static_cast<T>(0) or sign(rF) == sign(denom));
        return {.quot = qF, .rem = rF};
}

} // namespace xstd

// P3391 makes the delegated tuple formatter constexpr where supported.
#ifdef __cpp_lib_constexpr_format
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

template<class T>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2
struct std::formatter<xstd::div_t<T>> : std::formatter<std::tuple<T const&, T const&>>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_t<T> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                return std::formatter<std::tuple<T const&, T const&>>::format(std::tie(d.quot, d.rem), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_CSTDLIB_HPP
