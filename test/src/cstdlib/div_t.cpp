//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/div_t.hpp>          // div_t
#include <xstd/test/exact_width_types.hpp> // std_signed_types
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <concepts>                        // same_as
#include <cstdint>                         // int8_t
#include <utility>                         // declval

BOOST_AUTO_TEST_SUITE(CStdLib)

// div_t{q, r} spells the result of a call to div at the argument's own width,
// the way div_t/ldiv_t/lldiv_t/imaxdiv_t used to.
BOOST_AUTO_TEST_CASE(DeducedDivT)
{
        static_assert(std::same_as<decltype(xstd::div_t{1, 2}), xstd::div_t<int>>);
        static_assert(std::same_as<decltype(xstd::div_t{1L, 2L}), xstd::div_t<long>>);
        static_assert(std::same_as<decltype(xstd::div_t{std::int8_t{1}, std::int8_t{2}}), xstd::div_t<std::int8_t>>);

        BOOST_CHECK(true);
}

// The computed exception specification is the element's, which is what a
// written one would have to be kept equal to by hand.
BOOST_AUTO_TEST_CASE_TEMPLATE(EqualityTracksTheElementType, T, xstd::test::std_signed_types)
{
        static_assert(noexcept(std::declval<xstd::div_t<T> const&>() == std::declval<xstd::div_t<T> const&>()) ==
                      noexcept(std::declval<T const&>() == std::declval<T const&>()));

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
