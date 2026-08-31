//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer.hpp>          // integer
#include <xstd/test/exact_width_types.hpp>    // exact_width_integer_types
#include <xstd/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <boost/test/unit_test.hpp>
#include <concepts> // same_as
#include <cstdint>  // Boost.Test

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

// What keeps the character types out, in place of naming them: an integer is one
// of its own signed/unsigned pair, and a character type is a third type beside it.
BOOST_AUTO_TEST_CASE(SignedPairing)
{
        static_assert(std::same_as<xstd::make_signed_t<char>, signed char>);
        static_assert(std::same_as<xstd::make_unsigned_t<char>, unsigned char>);

        static_assert(not std::same_as<char, xstd::make_signed_t<char>>);
        static_assert(not std::same_as<char, xstd::make_unsigned_t<char>>);
        static_assert(not std::same_as<wchar_t, xstd::make_signed_t<wchar_t>>);
        static_assert(not std::same_as<wchar_t, xstd::make_unsigned_t<wchar_t>>);
        static_assert(not std::same_as<char8_t, xstd::make_signed_t<char8_t>>);
        static_assert(not std::same_as<char8_t, xstd::make_unsigned_t<char8_t>>);
        static_assert(not std::same_as<char16_t, xstd::make_signed_t<char16_t>>);
        static_assert(not std::same_as<char16_t, xstd::make_unsigned_t<char16_t>>);
        static_assert(not std::same_as<char32_t, xstd::make_signed_t<char32_t>>);
        static_assert(not std::same_as<char32_t, xstd::make_unsigned_t<char32_t>>);

        static_assert(std::same_as<signed char, xstd::make_signed_t<signed char>>);
        static_assert(std::same_as<unsigned char, xstd::make_unsigned_t<unsigned char>>);
        static_assert(std::same_as<int, xstd::make_signed_t<int>>);
        static_assert(std::same_as<unsigned, xstd::make_unsigned_t<unsigned>>);
        BOOST_CHECK(true);
}

// Both compositions are settled by signedness alone, whichever road is taken.
BOOST_AUTO_TEST_CASE_TEMPLATE(SignedRoundTrip, T, xstd::test::exact_width_integer_types)
{
        static_assert(std::same_as<xstd::make_signed_t<xstd::make_unsigned_t<T>>, xstd::make_signed_t<T>>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::make_signed_t<T>>, xstd::make_unsigned_t<T>>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
