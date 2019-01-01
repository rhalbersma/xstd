//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>         // to_underlying_type
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <type_traits>                  // integral_constant

using namespace xstd;

BOOST_AUTO_TEST_SUITE(TypeTraits)

enum class e1 {};
enum class e2 : bool {};
enum class e3 : char {};
enum class e4 : unsigned char {};
enum class e5 : unsigned {};
 
template<e1 N> using e1_ = std::integral_constant<e1, N>;
template<e2 N> using e2_ = std::integral_constant<e2, N>;
template<e3 N> using e3_ = std::integral_constant<e3, N>;
template<e4 N> using e4_ = std::integral_constant<e4, N>;
template<e5 N> using e5_ = std::integral_constant<e5, N>;

BOOST_AUTO_TEST_CASE(ToUnderlyingType)
{
        BOOST_CHECK_EQUAL(to_underlying_type(e1{}), 0);
        BOOST_CHECK_EQUAL(to_underlying_type(e2{}), false);
        BOOST_CHECK_EQUAL(to_underlying_type(e3{}), static_cast<char>(0));
        BOOST_CHECK_EQUAL(to_underlying_type(e4{}), static_cast<unsigned char>(0));
        BOOST_CHECK_EQUAL(to_underlying_type(e5{}), static_cast<unsigned>(0));

        BOOST_CHECK_EQUAL(to_underlying_type(e1_<e1{}>{}), 0);
        BOOST_CHECK_EQUAL(to_underlying_type(e2_<e2{}>{}), false);
        BOOST_CHECK_EQUAL(to_underlying_type(e3_<e3{}>{}), static_cast<char>(0));
        BOOST_CHECK_EQUAL(to_underlying_type(e4_<e4{}>{}), static_cast<unsigned char>(0));
        BOOST_CHECK_EQUAL(to_underlying_type(e5_<e5{}>{}), static_cast<unsigned>(0));
}

BOOST_AUTO_TEST_SUITE_END()
