//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>         // is_specialization_of, is_integral_constant
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <complex>                      // complex
#include <type_traits>                  // integral_constant

using namespace xstd;

BOOST_AUTO_TEST_SUITE(TypeTraits)

template<class T>
using is_complex = is_specialization_of<T, std::complex>;

template<class T>
inline constexpr auto is_complex_v = is_complex<T>::value;

BOOST_AUTO_TEST_CASE(IsSpecializationOf)
{
        BOOST_CHECK((    is_complex_v<std::complex<int>>));
        BOOST_CHECK((not is_complex_v<int>));
}

template<int N>
using int_ = std::integral_constant<int, N>;

BOOST_AUTO_TEST_CASE(IsIntegralConstant)
{
        BOOST_CHECK((    is_integral_constant_v<std:: true_type, bool>));
        BOOST_CHECK((    is_integral_constant_v<std::false_type, bool>));
        BOOST_CHECK((not is_integral_constant_v<bool, bool>));

        BOOST_CHECK((    is_integral_constant_v<int_<0>, int>));
        BOOST_CHECK((not is_integral_constant_v<int,     int>));
}

BOOST_AUTO_TEST_SUITE_END()
