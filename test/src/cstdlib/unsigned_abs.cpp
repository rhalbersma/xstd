//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/unsigned_abs.hpp>   // unsigned_abs
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <limits>                          // numeric_limits
#include <xstd/type_traits.hpp>            // make_unsigned_like_t

BOOST_AUTO_TEST_SUITE(CStdLib)

// The MIN-boundary wraparound directly and at compile time: what distinguishes
// unsigned_abs from abs, and what a widening-based |x| could not have handled.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedAbs, T, xstd::test::exact_width_signed_types)
{
        using U = xstd::make_unsigned_like_t<T>;

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{-2}), U{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{0}), U{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{+2}), U{2});

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::min()), static_cast<U>(static_cast<U>(limits::max()) + U{1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::max()), static_cast<U>(limits::max()));
}

BOOST_AUTO_TEST_SUITE_END()
