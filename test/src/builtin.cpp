#include <xstd/builtin.hpp>
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <limits>                               // digits

BOOST_AUTO_TEST_SUITE(Builtin)

template<class T> constexpr auto mask_none =  static_cast<T>(0);
template<class T> constexpr auto mask_one  =  static_cast<T>(1);
template<class T> constexpr auto mask_all  = ~static_cast<T>(0);

// the gcc intrinsics are defined for the following types
using UnsignedIntegerTypes = boost::mpl::vector
<
        unsigned,
        unsigned long,
        unsigned long long,
        __uint128_t
>;

using namespace xstd::builtin;

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(ctznz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(ctznz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZerosNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(clznz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(clznz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popcount, T, UnsignedIntegerTypes)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = ~(mask_all<T> << i);
                BOOST_CHECK_EQUAL(popcount(b), i);
        }

        BOOST_CHECK_EQUAL(popcount(mask_all<T>), std::numeric_limits<T>::digits);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForwardNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsfnz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsfnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverseNonZero, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsrnz(mask_all<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsrnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(ctz(mask_none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(ctz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(ctz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZeros, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(clz(mask_none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(clz(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(clz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForward, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsf(mask_none<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(bsf(mask_all<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsf(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverse, T, UnsignedIntegerTypes)
{
        BOOST_CHECK_EQUAL(bsr(mask_none<T>), -1);
        BOOST_CHECK_EQUAL(bsr(mask_all<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = mask_one<T> << i;
                BOOST_CHECK_EQUAL(bsr(b), i);
        }
}

BOOST_AUTO_TEST_SUITE_END()
