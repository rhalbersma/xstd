//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/unsigned_integer_like.hpp> // unsigned_integer_like
#include <xstd/test/exact_width_types.hpp>         // exact-width integer types
#include <boost/test/unit_test.hpp>                // Boost.Test

BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedIntegerLike, T, xstd::test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::unsigned_integer_like<T>);
        static_assert(xstd::unsigned_integer_like<T const>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_CASE_TEMPLATE(SignedIsNotUnsigned, T, xstd::test::exact_width_signed_integer_types)
{
        static_assert(not xstd::unsigned_integer_like<T>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_SUITE_END()
