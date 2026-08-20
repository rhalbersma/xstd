//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/void_type.hpp> // void_type
#include <xstd/test/constexpr_check.hpp>  // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>       // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                       // equality_comparable, three_way_comparable
#include <type_traits>                    // is_empty_v, is_nothrow_default_constructible_v, is_trivially_default_constructible_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(VoidTypeIsEmpty)
{
        XSTD_CONSTEXPR_CHECK(std::is_empty_v<xstd::void_type>);
}

BOOST_AUTO_TEST_CASE(VoidTypeIsDefaultConstructible)
{
        XSTD_CONSTEXPR_CHECK(std::is_trivially_default_constructible_v<xstd::void_type>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_default_constructible_v<xstd::void_type>);
}

// The difference from empty_type, which is regular and totally ordered so that an
// enclosing class can default its comparisons over it. A base says nothing, and a
// class that inherits it does not become comparable by doing so.
BOOST_AUTO_TEST_CASE(VoidTypeIsNotComparable)
{
        XSTD_CONSTEXPR_CHECK((not std::equality_comparable<xstd::void_type>));
        XSTD_CONSTEXPR_CHECK((not std::three_way_comparable<xstd::void_type>));

        struct derived : xstd::void_type
        {};

        XSTD_CONSTEXPR_CHECK((not std::equality_comparable<derived>));
        XSTD_CONSTEXPR_CHECK((not std::three_way_comparable<derived>));
}

BOOST_AUTO_TEST_SUITE_END()
