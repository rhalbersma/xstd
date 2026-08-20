//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/unsigned_abs.hpp>            // unsigned_abs
#include <xstd/test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <xstd/test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <xstd/limits.hpp>                          // numeric_limits
#include <boost/test/unit_test.hpp>                 // Boost.Test
#include <concepts>                                 // same_as
#include <xstd/type_traits.hpp>                     // make_unsigned_t

BOOST_AUTO_TEST_SUITE(CStdLib)

// The MIN-boundary wraparound, at compile time: what distinguishes unsigned_abs from abs.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedAbs, T, xstd::test::exact_width_signed_integer_types)
{
        using U = xstd::make_unsigned_t<T>;

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{-2}), U{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{0}), U{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{+2}), U{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::min()), static_cast<U>(static_cast<U>(limits::max()) + U{1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::max()), static_cast<U>(limits::max()));
}

// The identity, and its own counterpart, so that boundary is not a boundary here.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedAbsUnsigned, T, xstd::test::exact_width_unsigned_integer_types)
{
        static_assert(std::same_as<xstd::make_unsigned_t<T>, T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), T>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{0}), T{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{2}), T{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::min()), limits::min());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::max()), limits::max());
}

BOOST_AUTO_TEST_SUITE_END()
