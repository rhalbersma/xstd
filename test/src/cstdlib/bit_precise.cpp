//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/test/bit_precise.hpp>          // XSTD_TEST_HAS_BIT_PRECISE, bit_int, bit_uint
#include <xstd/test/exact_width_types.hpp>    // bit_precise_signed_types, bit_precise_unsigned_types
#include <xstd/cstdlib.hpp>                   // abs, div, div_euclid, div_floor, sign, unsigned_abs
#include <xstd/concepts.hpp>                  // integer_class, integer
#include <xstd/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <xstd/limits.hpp>                    // numeric_limits
#include <boost/test/unit_test.hpp>           // Boost.Test
#include <concepts>                           // same_as
#include <limits>                             // numeric_limits

BOOST_AUTO_TEST_SUITE(CStdLib)

#ifdef XSTD_TEST_HAS_BIT_PRECISE

namespace {

// Truncated: the quotient identity, the remainder bound, and the numerator's sign.
template<class T>
auto check_truncated(T numer, T denom) -> void
{
        auto const [quotient, remainder] = xstd::div(numer, denom);
        BOOST_CHECK((quotient * denom) + remainder == numer);
        BOOST_CHECK(xstd::unsigned_abs(remainder) < xstd::unsigned_abs(denom));
        BOOST_CHECK(remainder == T(0) or xstd::sign(remainder) == xstd::sign(numer));
}

// Floored: the same identity, and a nonzero remainder carrying the denominator's sign.
template<class T>
auto check_floored(T numer, T denom) -> void
{
        auto const [quotient, remainder] = xstd::div_floor(numer, denom);
        BOOST_CHECK((quotient * denom) + remainder == numer);
        BOOST_CHECK(remainder == T(0) or xstd::sign(remainder) == xstd::sign(denom));
}

// Euclidean: the same identity, and a remainder that is never negative.
template<class T>
auto check_euclidean(T numer, T denom) -> void
{
        auto const [quotient, remainder] = xstd::div_euclid(numer, denom);
        BOOST_CHECK((quotient * denom) + remainder == numer);
        BOOST_CHECK(xstd::sign(remainder) >= 0);
}

// Every ordered pair the type can hold, which only a width this narrow makes affordable.
template<class T>
auto sweep() -> void
{
        using limits = xstd::numeric_limits<T>;
        auto const lo = static_cast<int>(limits::min());
        auto const hi = static_cast<int>(limits::max());
        for (auto n = lo; n <= hi; ++n) {
                for (auto d = lo; d <= hi; ++d) {
                        // The two the contract excludes, rather than the two it gets wrong.
                        if (d == 0 or (n == lo and d == -1)) {
                                continue;
                        }
                        check_truncated(T(n), T(d));
                        check_floored(T(n), T(d));
                        check_euclidean(T(n), T(d));
                }
        }
}

} // namespace

// What no other width affords: the postconditions over the complete value space, not a sample.
BOOST_AUTO_TEST_CASE(DivisionOverEveryPair)
{
        sweep<xstd::test::bit_uint<2>>();
        sweep<xstd::test::bit_uint<3>>();
        sweep<xstd::test::bit_uint<4>>();
        sweep<xstd::test::bit_int<2>>();
        sweep<xstd::test::bit_int<3>>();
        sweep<xstd::test::bit_int<4>>();
}

// And at every width in the list, the values a sweep of 256 bits could never reach.
BOOST_AUTO_TEST_CASE_TEMPLATE(SignedExtremes, T, xstd::test::bit_precise_signed_types)
{
        using limits = xstd::numeric_limits<T>;
        using U = xstd::make_unsigned_t<T>;

        BOOST_CHECK(xstd::sign(limits::min()) == -1);
        BOOST_CHECK(xstd::sign(limits::max()) == +1);
        BOOST_CHECK(xstd::abs(static_cast<T>(limits::min() + T(1))) == limits::max());

        // The one magnitude no signed width holds, which is why the counterpart returns it.
        BOOST_CHECK(xstd::unsigned_abs(limits::min()) == static_cast<U>(limits::max()) + U(1));
}

// The unsigned extremes at every configured native width.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedExtremes, T, xstd::test::bit_precise_unsigned_types)
{
        using limits = xstd::numeric_limits<T>;

        BOOST_CHECK(xstd::sign(limits::min()) == 0);
        BOOST_CHECK(xstd::sign(limits::max()) == +1);
        BOOST_CHECK(xstd::abs(limits::max()) == limits::max());
        BOOST_CHECK(xstd::unsigned_abs(limits::max()) == limits::max());
        BOOST_CHECK(xstd::div(limits::max(), limits::max()).quotient == T(1));
}

#else

// The lists are empty without the storage, and Boost.Test wants a case either way.
BOOST_AUTO_TEST_CASE(NoBitPreciseStorage)
{
        BOOST_CHECK(std::tuple_size_v<xstd::test::bit_precise_signed_types> == 0);
}

#endif // XSTD_TEST_HAS_BIT_PRECISE

BOOST_AUTO_TEST_SUITE_END()
