#pragma once
#include <xstd/bit/mask.hpp>                    // all, any, none
#include <xstd/cstddef.hpp>                     // _z
#include <xstd/limits.hpp>                      // digits
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END

namespace xstd {
namespace bit {

BOOST_AUTO_TEST_SUITE(BitIntrinsic)

// the gcc intrinsics are defined for the following types
using UnsignedIntegerTypes = boost::mpl::vector
<
        unsigned,
        unsigned long,
        unsigned long long
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::ctz(mask::none<T>), digits<T>);
        BOOST_CHECK_EQUAL(lib::ctz(mask::all<T>), 0);

        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::ctz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::ctznz(mask::all<T>), 0);

        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::ctznz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForwardNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::bsfnz(mask::all<T>), 0);

        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::bsfnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::clz(mask::none<T>), digits<T>);
        BOOST_CHECK_EQUAL(lib::clz(mask::all<T>), 0);

        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::clz(b), digits<T> - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::clznz(mask::all<T>), 0);

        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::clznz(b), digits<T> - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverseNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(lib::bsrnz(mask::all<T>), digits<T> - 1);

        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = mask::one<T> << i;
                BOOST_CHECK_EQUAL(lib::bsrnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popcount, T, UnsignedIntegerTypes)
{
        for (auto i = 0_z; i < digits<T>; ++i) {
                auto const b = ~(mask::all<T> << i);
                BOOST_CHECK_EQUAL(lib::popcount(b), i);
        }

        BOOST_CHECK_EQUAL(lib::popcount(mask::all<T>), digits<T>);
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace bit
}       // namespace xstd
