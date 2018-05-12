//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>                     // is_representable_v
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <limits>                               // min, max

BOOST_AUTO_TEST_SUITE(CStdInt)

using namespace xstd;

using FixedWidthUnsignedIntegerTypes = boost::mpl::vector
<       uint8_t
,       uint16_t
,       uint32_t
,       uint64_t
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(MinMaxAreSelfRepresentable, T, FixedWidthUnsignedIntegerTypes)
{
        static_assert(is_representable_v<T, T, std::numeric_limits<T>::min()>);
        static_assert(is_representable_v<T, T, std::numeric_limits<T>::max()>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(MaxIsSelfRepresented, T, FixedWidthUnsignedIntegerTypes)
{
        static_assert(std::is_same_v<uint_least_t<T, std::numeric_limits<T>::max()>, T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UIntLeasTIsSmallerThanUIntFast, T, FixedWidthUnsignedIntegerTypes)
{
        static_assert(sizeof(uint_least_t<T, std::numeric_limits<T>::max()>) <= sizeof(uint_fast_t<T, std::numeric_limits<T>::max()>));
}

BOOST_AUTO_TEST_SUITE_END()
