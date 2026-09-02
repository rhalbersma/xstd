//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/type_traits.hpp>       // the sign traits and modifiers
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                        // same_as

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(TypeTraits)

// The five together: the traits say which half a type is in, the modifiers move it, and neither header sees the other.
template<class T>
concept consistent =
        // An integer is in exactly one half.
        (xstd::is_signed_v<T> != xstd::is_unsigned_v<T>) and
        // The modifiers land in the half they name, whichever half they started in.
        xstd::is_signed_v<xstd::make_signed_t<T>> and
        xstd::is_unsigned_v<xstd::make_unsigned_t<T>> and
        // And they are mutually inverse, so the two halves are the same size.
        std::same_as<xstd::make_signed_t<xstd::make_unsigned_t<T>>, xstd::make_signed_t<T>> and
        std::same_as<xstd::make_unsigned_t<xstd::make_signed_t<T>>, xstd::make_unsigned_t<T>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(TheTraitsAndTheModifiersAgree, T, xstd::test::exact_width_signed_integer_types)
{
        static_assert(consistent<T>);
        static_assert(consistent<xstd::make_unsigned_t<T>>);
        static_assert(std::same_as<xstd::make_signed_t<T>, T>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TheTraitsAndTheModifiersAgreeUnsigned, T, xstd::test::exact_width_unsigned_integer_types)
{
        static_assert(consistent<T>);
        static_assert(consistent<xstd::make_signed_t<T>>);
        static_assert(std::same_as<xstd::make_unsigned_t<T>, T>);
        BOOST_CHECK(true);
}

// promoted is the odd one out: it moves width, not sign, so it must leave the half alone.
BOOST_AUTO_TEST_CASE_TEMPLATE(PromotionKeepsASignedTypeSigned, T, xstd::test::exact_width_signed_integer_types)
{
        static_assert(xstd::is_signed_v<xstd::promoted_t<T>>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
