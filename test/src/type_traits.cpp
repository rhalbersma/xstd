//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>            // sign traits and modifiers
#include <xstd/test/exact_width_types.hpp> // exact-width integer types
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <concepts>                        // same_as

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE_TEMPLATE(SignedTraitIdentities, T, xstd::test::exact_width_signed_integer_types)
{
        using U = xstd::make_unsigned_t<T>;
        static_assert(xstd::is_signed_v<T> and not xstd::is_unsigned_v<T>);
        static_assert(xstd::is_unsigned_v<U> and not xstd::is_signed_v<U>);
        static_assert(std::same_as<xstd::make_signed_t<U>, T>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedTraitIdentities, T, xstd::test::exact_width_unsigned_integer_types)
{
        using S = xstd::make_signed_t<T>;
        static_assert(xstd::is_unsigned_v<T> and not xstd::is_signed_v<T>);
        static_assert(xstd::is_signed_v<S> and not xstd::is_unsigned_v<S>);
        static_assert(std::same_as<xstd::make_unsigned_t<S>, T>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
