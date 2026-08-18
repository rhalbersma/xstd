//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer_like.hpp>  // integer_like
#include <xstd/test/exact_width_types.hpp> // exact_width_integer_types
#include <boost/test/unit_test.hpp>        // Boost.Test

BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_CASE_TEMPLATE(IntegerLike, T, xstd::test::exact_width_integer_types)
{
        static_assert(xstd::integer_like<T>);
        static_assert(xstd::integer_like<T const>);
        static_assert(xstd::integer_like<T volatile>);
        static_assert(xstd::integer_like<T const volatile>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_CASE(RejectsNonIntegers)
{
        static_assert(not xstd::integer_like<bool>);
        static_assert(not xstd::integer_like<double>);
        static_assert(not xstd::integer_like<void>);
        static_assert(not xstd::integer_like<int*>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_SUITE_END()
