//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>         // abs, uabs, sign, div_t, div, euclidean_div, floored_div
#include <xstd/concepts.hpp>        // signed_integral_like, unsigned_integral_like
#include <xstd/type_traits.hpp>     // make_unsigned_like, make_unsigned_like_t
#include <absl/numeric/int128.h>    // int128, uint128
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <concepts>                 // integral, same_as
#include <limits>                   // numeric_limits

// The second third-party integer-class type, after Boost.Multiprecision. Two
// independent libraries satisfying xstd::signed_integral_like unmodified says
// something a single one cannot: that the concept describes the category
// rather than either implementation's habits.
//
// Where the two differ is the representation, and that is the reason to carry
// both. absl::int128 is *two's complement*, so its range is asymmetric -
// min() == -max() - 1 - exactly like the built-in widths and __int128, and
// unlike cpp_int's signed-magnitude types. Every expected value below is
// therefore the same one test/src/cstdlib.cpp writes for the built-ins,
// max() and all, where test/src/multiprecision.cpp has to derive its
// MIN-boundary values from the type's own bounds.
//
// This file is the one part of the test suite that needs a *compiled*
// third-party library rather than headers alone: absl::int128's division is
// out of line. If Abseil ever fails to build on a toolchain in the matrix,
// turn XSTD_TEST_ABSEIL off in test/CMakeLists.txt - the coverage lost is the
// second witness, not the only one.
template<>
struct xstd::make_unsigned_like<absl::int128>
{
        using type = absl::uint128;
};

BOOST_AUTO_TEST_SUITE(Abseil)

using T = absl::int128;
using U = absl::uint128;
using limits = std::numeric_limits<T>;

// Runtime rather than XSTD_CONSTEXPR_CHECK, for the same reason as the
// multiprecision battery: absl::int128's operator/ is not constexpr, so the
// static_assert half of that macro cannot be formed. The constraint checks
// above it are compile-time regardless, since a concept needs no evaluation.
BOOST_AUTO_TEST_CASE(SignedIntegralLike)
{
        // no class type is ever std::integral
        static_assert(not std::integral<T>);
        static_assert(xstd::signed_integral_like<T>);
        static_assert(xstd::unsigned_integral_like<U>);
        static_assert(not xstd::signed_integral_like<U>);
        static_assert(std::same_as<xstd::make_unsigned_like_t<T>, U>);
        static_assert(std::same_as<decltype(xstd::uabs(T{})), U>);
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);

        BOOST_CHECK((xstd::abs(T{-2}) == T{2}));
        BOOST_CHECK((xstd::abs(T{+2}) == T{2}));

        BOOST_CHECK((xstd::uabs(T{-2}) == U{2}));
        BOOST_CHECK((xstd::uabs(T{+2}) == U{2}));

        BOOST_CHECK((xstd::sign(T{-2}) == -1));
        BOOST_CHECK((xstd::sign(T{0}) == 0));
        BOOST_CHECK((xstd::sign(T{+2}) == +1));

        BOOST_CHECK((xstd::div(T{+8}, T{+3}) == xstd::div_t<T>{T{+2}, T{+2}}));
        BOOST_CHECK((xstd::div(T{-8}, T{+3}) == xstd::div_t<T>{T{-2}, T{-2}}));
        BOOST_CHECK((xstd::div(T{-8}, T{-3}) == xstd::div_t<T>{T{+2}, T{-2}}));

        // all three arms of euclidean_div's adjustment, and both of
        // floored_div's
        BOOST_CHECK((xstd::euclidean_div(T{+8}, T{+3}) == xstd::div_t<T>{T{+2}, T{+2}}));
        BOOST_CHECK((xstd::euclidean_div(T{-8}, T{+3}) == xstd::div_t<T>{T{-3}, T{+1}}));
        BOOST_CHECK((xstd::euclidean_div(T{-8}, T{-3}) == xstd::div_t<T>{T{+3}, T{+1}}));

        BOOST_CHECK((xstd::floored_div(T{+8}, T{+3}) == xstd::div_t<T>{T{+2}, T{+2}}));
        BOOST_CHECK((xstd::floored_div(T{-8}, T{+3}) == xstd::div_t<T>{T{-3}, T{+1}}));
        BOOST_CHECK((xstd::floored_div(T{-8}, T{-3}) == xstd::div_t<T>{T{+2}, T{-2}}));
}

// The asymmetric range, and the MIN boundary it makes reachable. These are
// the checks that read differently on a signed-magnitude type: there,
// |min()| is max() and the euclidean remainder is max() - 1.
BOOST_AUTO_TEST_CASE(TwosComplementRange)
{
        BOOST_CHECK((limits::min() != -limits::max()));
        BOOST_CHECK((limits::min() == static_cast<T>(-limits::max() - T{1})));

        // |min()| is one past max(), which is the whole reason uabs returns
        // the unsigned counterpart rather than T
        BOOST_CHECK((xstd::uabs(limits::min()) == static_cast<U>(static_cast<U>(limits::max()) + U{1})));
        BOOST_CHECK((xstd::uabs(limits::max()) == static_cast<U>(limits::max())));

        // denom == MIN is in contract for all three conventions
        BOOST_CHECK((xstd::div(T{-1}, limits::min()) == xstd::div_t<T>{T{0}, T{-1}}));
        BOOST_CHECK((xstd::euclidean_div(T{-1}, limits::min()) == xstd::div_t<T>{T{+1}, limits::max()}));
        BOOST_CHECK((xstd::floored_div(T{-1}, limits::min()) == xstd::div_t<T>{T{0}, T{-1}}));

        BOOST_CHECK((xstd::div(T{+1}, limits::min()) == xstd::div_t<T>{T{0}, T{+1}}));
        BOOST_CHECK((xstd::floored_div(T{+1}, limits::min()) == xstd::div_t<T>{T{-1}, static_cast<T>(limits::min() + T{1})}));

        // abs is out of contract at min() here, unlike on a symmetric range,
        // because |min()| does not fit in T. It is not called.
        BOOST_CHECK((xstd::abs(static_cast<T>(limits::min() + T{1})) == limits::max()));
}

BOOST_AUTO_TEST_SUITE_END()
