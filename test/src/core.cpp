//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/core.hpp>            // the whole core surface
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <type_traits>              // integral_constant, is_empty_v

BOOST_AUTO_TEST_SUITE(Core)

template<class>
struct box
{};

// concepts and type_traits state the same question two ways, so they must answer it the same way.
template<class T>
concept agrees = xstd::specialization_of<T, box> == xstd::is_specialization_of<T, box>::value;

BOOST_AUTO_TEST_CASE(TheConceptAndTheTraitAreOnePredicate)
{
        static_assert(agrees<box<int>>);
        static_assert(agrees<box<box<int>>>);
        static_assert(agrees<int>);
        static_assert(agrees<box<int>&>);
        static_assert(agrees<void>);

        // And they are not both vacuously false: the yes and the no are each reached.
        static_assert(xstd::specialization_of<box<int>, box>);
        static_assert(not xstd::specialization_of<int, box>);
        BOOST_CHECK(true);
}

// The tag empty_type carries is a type like any other, so the trait sees through the alias to it.
enum class shade : int { one = 1 };

BOOST_AUTO_TEST_CASE(TheUtilitiesMeetOverTheLibrarysOwnTemplates)
{
        static_assert(std::is_empty_v<xstd::empty_type<shade>>);
        static_assert(xstd::is_specialization_of<xstd::empty_type<shade>, xstd::empty_type>::value);
        static_assert(xstd::specialization_of<xstd::empty_type<shade>, xstd::empty_type>);
        static_assert(xstd::to_underlying(std::integral_constant<shade, shade::one>{}) == 1);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
