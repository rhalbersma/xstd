//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>         // any_of
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL
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

BOOST_AUTO_TEST_CASE(IsAnyOf)
{
        BOOST_CHECK((any_of<e1, e1, e2, e3, e4, e5>));
        BOOST_CHECK((any_of<e2, e1, e2, e3, e4, e5>));
        BOOST_CHECK((any_of<e3, e1, e2, e3, e4, e5>));
        BOOST_CHECK((any_of<e4, e1, e2, e3, e4, e5>));
        BOOST_CHECK((any_of<e5, e1, e2, e3, e4, e5>));
        BOOST_CHECK((!any_of<int, e1, e2, e3, e4, e5>));
        BOOST_CHECK((!any_of<bool, e1, e2, e3, e4, e5>));
        BOOST_CHECK((!any_of<char, e1, e2, e3, e4, e5>));
        BOOST_CHECK((!any_of<unsigned char, e1, e2, e3, e4, e5>));
        BOOST_CHECK((!any_of<unsigned, e1, e2, e3, e4, e5>));
}

BOOST_AUTO_TEST_SUITE_END()
