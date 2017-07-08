//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>                     // ctznz, clznz, popcount, bsfnz, bsrnz, ctz, clz, bsf, bsr, bit1
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <limits>                               // digits

BOOST_AUTO_TEST_SUITE(Builtin)

using UnsignedIntegerTypes = boost::mpl::vector
<
        unsigned,
        unsigned long,
        unsigned long long,
        __uint128_t
>;

using namespace xstd;

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::ctznz(detail::ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::ctznz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::clznz(detail::ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::clznz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popcount, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::popcount(detail::zero<T>), 0);
        BOOST_CHECK_EQUAL(detail::popcount(detail::ones<T>), std::numeric_limits<T>::digits);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = ~(detail::ones<T> << i);
                BOOST_CHECK_EQUAL(detail::popcount(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForwardNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::bsfnz(detail::ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::bsfnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverseNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::bsrnz(detail::ones<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::bsrnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::ctz(detail::zero<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(detail::ctz(detail::ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::ctz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::clz(detail::zero<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(detail::clz(detail::ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::clz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForward, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::bsf(detail::zero<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(detail::bsf(detail::ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::bsf(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverse, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(detail::bsr(detail::zero<T>), -1);
        BOOST_CHECK_EQUAL(detail::bsr(detail::ones<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::bsr(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Bit1, T, UnsignedIntegerTypes)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = detail::bit1<T>(i);
                BOOST_CHECK_EQUAL(detail::popcount(b), 1);

                // __uint128_t does not have an overloaded streaming output operator<<
                // BOOST_CHECK_EQUAL will confuse it with the bitwise left-shift operator<<
                BOOST_CHECK(b == static_cast<T>(1) << i);
        }
}

BOOST_AUTO_TEST_SUITE_END()
