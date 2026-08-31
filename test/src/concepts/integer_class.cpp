//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer_class.hpp> // integer_class
#include <xstd/test/exact_width_types.hpp> // exact_width_integer_types
#include <boost/test/unit_test.hpp>        // Boost.Test

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE_TEMPLATE(IntegerClass, T, xstd::test::exact_width_integer_types)
{
        static_assert(xstd::integer_class<T>);
        static_assert(xstd::integer_class<T const>);
        static_assert(xstd::integer_class<T volatile>);
        static_assert(xstd::integer_class<T const volatile>);
        BOOST_CHECK(true);
}

// The requirements hold of the types they were modelled on, which the old
// (not std::integral) clause made unaskable rather than false. `integer` now
// rests on this: it dropped its std::integral disjunct as redundant.
BOOST_AUTO_TEST_CASE(StandardIntegral)
{
        static_assert(xstd::integer_class<char>);
        static_assert(xstd::integer_class<signed char>);
        static_assert(xstd::integer_class<unsigned char>);
        static_assert(xstd::integer_class<char8_t>);
        static_assert(xstd::integer_class<char16_t>);
        static_assert(xstd::integer_class<char32_t>);
        static_assert(xstd::integer_class<wchar_t>);
        static_assert(xstd::integer_class<short>);
        static_assert(xstd::integer_class<unsigned short>);
        static_assert(xstd::integer_class<int>);
        static_assert(xstd::integer_class<unsigned int>);
        static_assert(xstd::integer_class<long>);
        static_assert(xstd::integer_class<unsigned long>);
        static_assert(xstd::integer_class<long long>);
        static_assert(xstd::integer_class<unsigned long long>);

        // C++17 removed bool's ++ and --, which /7.1 and /7.2 ask for.
        static_assert(not xstd::integer_class<bool>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
