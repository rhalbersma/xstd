//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/test/exact_width_types.hpp> // bit_precise_signed_types, bit_precise_unsigned_types
#include <xstd/cstdlib.hpp>                // abs, div, euclidean_div, floored_div, sign, unsigned_abs
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <limits>                          // numeric_limits

BOOST_AUTO_TEST_SUITE(CStdLib)

#ifdef XSTD_TEST_HAS_BIT_PRECISE

namespace {

// Every ordered pair the type can hold, which only a width this narrow makes affordable.
template<class T>
auto sweep() -> void
{
        using limits = std::numeric_limits<T>;
        auto const lo = static_cast<int>(limits::min());
        auto const hi = static_cast<int>(limits::max());
        for (auto n = lo; n <= hi; ++n) {
                for (auto d = lo; d <= hi; ++d) {
                        // The two the contract excludes, rather than the two it gets wrong.
                        if (d == 0 or (n == lo and d == -1)) {
                                continue;
                        }
                        auto const numer = T(n);
                        auto const denom = T(d);

                        auto const [qT, rT] = xstd::div(numer, denom);
                        BOOST_CHECK(qT * denom + rT == numer);
                        BOOST_CHECK(xstd::unsigned_abs(rT) < xstd::unsigned_abs(denom));
                        BOOST_CHECK(rT == T(0) or xstd::sign(rT) == xstd::sign(numer));

                        auto const [qF, rF] = xstd::floored_div(numer, denom);
                        BOOST_CHECK(qF * denom + rF == numer);
                        BOOST_CHECK(rF == T(0) or xstd::sign(rF) == xstd::sign(denom));

                        auto const [qE, rE] = xstd::euclidean_div(numer, denom);
                        BOOST_CHECK(qE * denom + rE == numer);
                        BOOST_CHECK(xstd::sign(rE) >= 0);
                }
        }
}

} // namespace

// What no other width affords: the postconditions over the complete value space, not a sample.
BOOST_AUTO_TEST_CASE(DivisionOverEveryPair)
{
        sweep<xstd::test::bit_uint<1>>();
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
        using limits = std::numeric_limits<T>;
        using U = xstd::make_unsigned_like_t<T>;

        BOOST_CHECK(xstd::sign(limits::min()) == -1);
        BOOST_CHECK(xstd::sign(limits::max()) == +1);
        BOOST_CHECK(xstd::abs(static_cast<T>(limits::min() + T(1))) == limits::max());

        // The one magnitude no signed width holds, which is why the counterpart returns it.
        BOOST_CHECK(xstd::unsigned_abs(limits::min()) == static_cast<U>(limits::max()) + U(1));
}

// One bit is a width here, and the list starts there: it is the unsigned list that can.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedExtremes, T, xstd::test::bit_precise_unsigned_types)
{
        using limits = std::numeric_limits<T>;

        BOOST_CHECK(xstd::sign(limits::min()) == 0);
        BOOST_CHECK(xstd::sign(limits::max()) == +1);
        BOOST_CHECK(xstd::abs(limits::max()) == limits::max());
        BOOST_CHECK(xstd::unsigned_abs(limits::max()) == limits::max());
        BOOST_CHECK(xstd::div(limits::max(), limits::max()).quot == T(1));
}

#else

// The lists are empty without the storage, and Boost.Test wants a case either way.
BOOST_AUTO_TEST_CASE(NoBitPreciseStorage)
{
        BOOST_CHECK(std::tuple_size_v<xstd::test::bit_precise_signed_types> == 0);
}

#endif // XSTD_TEST_HAS_BIT_PRECISE

BOOST_AUTO_TEST_SUITE_END()
