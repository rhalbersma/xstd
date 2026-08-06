//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>         // int128_t, uint128_t
#include <xstd/format/int128.hpp>   // detail::decimal_buffer_size, detail::to_decimal
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK_EQUAL
#include <cstdint>                  // exact-width integer types
#include <limits>                   // numeric_limits
#include <string_view>              // string_view
#include <tuple>                    // tuple

BOOST_AUTO_TEST_SUITE(FormatInt128)

// A constant expression, not merely a constexpr function called at runtime:
// that is the property std::formatter<div_t<S>>::format needs from it in order
// to opt into P3391, and the one a std::to_string-based version cannot have.
template<xstd::integral_like T>
[[nodiscard]] consteval auto rendered(T value, std::string_view expected)
        -> bool
{
        char buffer[xstd::detail::decimal_buffer_size<T>];
        return std::string_view{xstd::detail::to_decimal(buffer, value)} == expected;
}

BOOST_AUTO_TEST_CASE(Decimal)
{
        static_assert(rendered(xstd::int128_t{0}, "0"));
        static_assert(rendered(xstd::int128_t{-1}, "-1"));
        static_assert(rendered(xstd::uint128_t{0}, "0"));
        static_assert(rendered(std::numeric_limits<xstd::int128_t>::min(), "-170141183460469231731687303715884105728"));
        static_assert(rendered(std::numeric_limits<xstd::int128_t>::max(), "170141183460469231731687303715884105727"));
        static_assert(rendered(std::numeric_limits<xstd::uint128_t>::max(), "340282366920938463463374607431768211455"));

        char buffer[xstd::detail::decimal_buffer_size<xstd::int128_t>];
        BOOST_CHECK_EQUAL(xstd::detail::to_decimal(buffer, std::numeric_limits<xstd::int128_t>::min()),
                          "-170141183460469231731687303715884105728");
}

// The narrow widths go through the same template, so the buffer has to be big
// enough at every one of them - digits10 undercounts by one by construction.
using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t, xstd::int128_t>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BoundariesFitTheBuffer, T, exact_width_types)
{
        char lo[xstd::detail::decimal_buffer_size<T>];
        char hi[xstd::detail::decimal_buffer_size<T>];
        auto const min = std::numeric_limits<T>::min();
        auto const max = std::numeric_limits<T>::max();

        // Writing backwards from the end, the first character written must
        // still land inside the buffer.
        BOOST_CHECK_GE(xstd::detail::to_decimal(lo, min), static_cast<char const*>(lo));
        BOOST_CHECK_GE(xstd::detail::to_decimal(hi, max), static_cast<char const*>(hi));
}

BOOST_AUTO_TEST_SUITE_END()
