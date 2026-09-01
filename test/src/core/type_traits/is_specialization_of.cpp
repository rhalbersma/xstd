//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/core/type_traits/is_specialization_of.hpp> // is_specialization_of
#include <xstd/test/constexpr_check.hpp>                  // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                        // complex

BOOST_AUTO_TEST_SUITE(Core)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSpecializationOf)

template<class T>
using is_complex = xstd::is_specialization_of<T, std::complex>;

template<class T>
inline constexpr auto is_complex_v = is_complex<T>::value;

BOOST_AUTO_TEST_CASE(HoldsOnlyForSpecializationsOfThePrimaryTemplate)
{
        XSTD_CONSTEXPR_CHECK((is_complex_v<std::complex<int>>));
        XSTD_CONSTEXPR_CHECK((not is_complex_v<int>));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
