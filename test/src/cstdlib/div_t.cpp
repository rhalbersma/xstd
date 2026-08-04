//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib/div_t.hpp>   // div_t
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp> // Boost.Test
#include <concepts>                 // same_as
#include <cstdint>                  // int8_t
#include <tuple>                    // tuple

BOOST_AUTO_TEST_SUITE(CStdLib)

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

// Class template argument deduction: div_t{q, r} still spells the result of
// a call to div at the argument's own width, the way the four separate
// div_t/ldiv_t/lldiv_t/imaxdiv_t names used to.
BOOST_AUTO_TEST_CASE(DeducedDivT)
{
        static_assert(std::same_as<decltype(xstd::div_t{1, 2}), xstd::div_t<int>>);
        static_assert(std::same_as<decltype(xstd::div_t{1L, 2L}), xstd::div_t<long>>);
        static_assert(std::same_as<decltype(xstd::div_t{std::int8_t{1}, std::int8_t{2}}), xstd::div_t<std::int8_t>>);

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
