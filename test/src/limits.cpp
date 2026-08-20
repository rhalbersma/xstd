//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/limits.hpp>               // numeric_limits
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                      // derived_from
#include <limits>                        // numeric_limits

BOOST_AUTO_TEST_SUITE(Limits)

BOOST_AUTO_TEST_CASE(DelegatesToTheStandardDomain)
{
        static_assert(std::derived_from<xstd::numeric_limits<int>, std::numeric_limits<int>>);
        static_assert(std::derived_from<xstd::numeric_limits<int const>, std::numeric_limits<int>>);
        static_assert(std::derived_from<xstd::numeric_limits<unsigned volatile>, std::numeric_limits<unsigned>>);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::numeric_limits<int>::min(), std::numeric_limits<int>::min());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::numeric_limits<int>::max(), std::numeric_limits<int>::max());
}

// A named concept, not a bare requires-expression: the trait is empty out there,
// so naming a member is ill-formed rather than false.
template<class T>
concept has_limits = requires { xstd::numeric_limits<T>::is_specialized; };

BOOST_AUTO_TEST_CASE(SaysNothingOutsideThatDomain)
{
        static_assert(has_limits<int>);
        static_assert(not has_limits<void>);
        static_assert(not has_limits<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        static_assert(not has_limits<int[3]>);
        static_assert(not has_limits<void()>);
}

BOOST_AUTO_TEST_SUITE_END()
