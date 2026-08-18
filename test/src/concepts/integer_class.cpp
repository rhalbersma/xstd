//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer_class.hpp> // integer_class
#include <xstd/test/exact_width_types.hpp> // exact_width_integer_types
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <concepts>                        // integral

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE_TEMPLATE(IntegerClass, T, xstd::test::exact_width_integer_types)
{
        static_assert(xstd::integer_class<T> == (not std::integral<T>));
        static_assert(xstd::integer_class<T const> == (not std::integral<T>));
        static_assert(xstd::integer_class<T volatile> == (not std::integral<T>));
        static_assert(xstd::integer_class<T const volatile> == (not std::integral<T>));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
