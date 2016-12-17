#pragma once
#include <xstd/int_set/mask.hpp>                // all, any, none
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <limits>                               // digits

namespace xstd {
namespace bit {

BOOST_AUTO_TEST_SUITE(BitIntrinsic)

// the gcc intrinsics are defined for the following types
using UnsignedIntegerTypes = boost::mpl::vector
<
        unsigned,
        unsigned long,
        unsigned long long,
        __uint128_t
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::ctznz(mask::all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::ctznz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::clznz(mask::all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::clznz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popcount, T, UnsignedIntegerTypes)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = ~(mask::all<T> << i);
                BOOST_CHECK_EQUAL(lib::popcount(b), i);
        }

        BOOST_CHECK_EQUAL(lib::popcount(mask::all<T>), std::numeric_limits<T>::digits);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForwardNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::bsfnz(mask::all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::bsfnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverseNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::bsrnz(mask::all<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::bsrnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::ctz(mask::none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(lib::ctz(mask::all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::ctz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::clz(mask::none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(lib::clz(mask::all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::clz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForward, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::bsf(mask::none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(lib::bsf(mask::all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::bsf(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverse, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::bsr(mask::none<T>), -1);
        BOOST_CHECK_EQUAL(lib::bsr(mask::all<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::bsr(b), i);
        }
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace bit
}       // namespace xstd
