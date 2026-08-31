//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer.hpp>       // integer
#include <xstd/test/exact_width_types.hpp> // exact_width_integer_types
#include <boost/test/unit_test.hpp>
#include <cstdint> // Boost.Test

template<class T>
concept cv_rejected_integer =
        not xstd::integer<T> and not xstd::integer<T const> and not xstd::integer<T volatile> and
        not xstd::integer<T const volatile>;

BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_CASE_TEMPLATE(Integer, T, xstd::test::exact_width_integer_types)
{
        static_assert(xstd::integer<T>);
        static_assert(xstd::integer<T const>);
        static_assert(xstd::integer<T volatile>);
        static_assert(xstd::integer<T const volatile>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_CASE(RejectsNonIntegers)
{
        static_assert(xstd::integer<signed char>);
        static_assert(xstd::integer<unsigned char>);
        static_assert(xstd::integer<std::int8_t>);
        static_assert(xstd::integer<std::uint8_t>);
        static_assert(cv_rejected_integer<char>);
        static_assert(cv_rejected_integer<wchar_t>);
        static_assert(cv_rejected_integer<char8_t>);
        static_assert(cv_rejected_integer<char16_t>);
        static_assert(cv_rejected_integer<char32_t>);
        // Excluded by integer_class alone, C++17 having removed bool's ++ and --.
        static_assert(cv_rejected_integer<bool>);
        static_assert(not xstd::integer<double>);
        static_assert(not xstd::integer<void>);
        static_assert(not xstd::integer<int*>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_SUITE_END()
