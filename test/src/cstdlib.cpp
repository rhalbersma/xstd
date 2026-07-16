//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>             // div, floored_div, euclidean_div
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_REQUIRE_EQUAL
#include <algorithm>                    // transform
#include <array>                        // array
#include <cstdlib>                      // div, div_t
#include <format>                       // format
#include <iterator>                     // back_inserter
#include <limits>                       // numeric_limits
#include <string>                       // string
#include <tuple>                        // tuple
#include <utility>                      // pair
#include <vector>                       // vector

// The whole point of these functions (p0533-style) is being usable in
// constant expressions; verify that at compile time.
static_assert(xstd::abs(-2) == 2);
static_assert(xstd::sign(-2) == -1);
static_assert(xstd::div(+8, +3) == xstd::div_t{+2, +2});
static_assert(xstd::euclidean_div(-8, +3) == xstd::div_t{-3, +1});
static_assert(xstd::floored_div(-8, +3) == xstd::div_t{-3, +1});
constexpr auto tuple_input = xstd::div_t{+2, -2};
static_assert(xstd::as_tuple(tuple_input) == std::tuple{+2, -2});

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE(Abs)
{
        BOOST_CHECK_EQUAL(xstd::abs(-2), 2);
        BOOST_CHECK_EQUAL(xstd::abs(-1), 1);
        BOOST_CHECK_EQUAL(xstd::abs( 0), 0);
        BOOST_CHECK_EQUAL(xstd::abs(+1), 1);
        BOOST_CHECK_EQUAL(xstd::abs(+2), 2);
}

BOOST_AUTO_TEST_CASE(Sign)
{
        BOOST_CHECK_EQUAL(xstd::sign(-2), -1);
        BOOST_CHECK_EQUAL(xstd::sign(-1), -1);
        BOOST_CHECK_EQUAL(xstd::sign( 0),  0);
        BOOST_CHECK_EQUAL(xstd::sign(+1), +1);
        BOOST_CHECK_EQUAL(xstd::sign(+2), +1);
}

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

auto const input = std::array<std::pair<int, int>, 8>
{{
        {+8, +3}, {+8, -3}, {-8, +3}, {-8, -3},
        {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}
}};

void check_equal(xstd::div_t actual, xstd::div_t expected)
{
        BOOST_CHECK_EQUAL(actual.quot, expected.quot);
        BOOST_CHECK_EQUAL(actual.rem, expected.rem);
}

template<class Sequence1, class Sequence2>
void check_equal_ranges(Sequence1 const& actual, Sequence2 const& expected)
{
        BOOST_REQUIRE_EQUAL(actual.size(), expected.size());
        for (auto i = 0uz; i != actual.size(); ++i) {
                check_equal(actual[i], expected[i]);
        }
}

BOOST_AUTO_TEST_CASE(StdDiv)
{
        auto const std_div = std::vector<xstd::div_t>
        {
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        };

        std::vector<xstd::div_t> std_res;
        std::transform(input.begin(), input.end(), std::back_inserter(std_res), [](auto const& p)
                -> xstd::div_t {
                auto const d = std::div(p.first, p.second);
                return { d.quot, d.rem };
        });

        check_equal_ranges(std_res, std_div);
}

BOOST_AUTO_TEST_CASE(TruncatedDiv)
{
        auto const div = std::vector<xstd::div_t>
        {
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        };

        std::vector<xstd::div_t> truncated_res;
        std::transform(input.begin(), input.end(), std::back_inserter(truncated_res), [](auto const& p) {
                return xstd::div(p.first, p.second);
        });

        check_equal_ranges(truncated_res, div);
}

BOOST_AUTO_TEST_CASE(EuclideanDiv)
{
        auto const euclidean_div = std::vector<xstd::div_t>
        {
                {+2, +2}, {-2, +2}, {-3, +1}, {+3, +1},
                { 0, +1}, { 0, +1}, {-1, +1}, {+1, +1}
        };

        std::vector<xstd::div_t> euclidean_res;
        std::transform(input.begin(), input.end(), std::back_inserter(euclidean_res), [](auto const& p) {
                return xstd::euclidean_div(p.first, p.second);
        });

        check_equal_ranges(euclidean_res, euclidean_div);
}


BOOST_AUTO_TEST_CASE(ExactDivisions)
{
        auto const exact_input = std::array<std::pair<int, int>, 4>
        {{
                {+6, +3}, {+6, -3}, {-6, +3}, {-6, -3}
        }};

        auto const expected = std::array<xstd::div_t, 4>
        {{
                {+2, 0}, {-2, 0}, {-2, 0}, {+2, 0}
        }};

        for (auto i = 0uz; i != exact_input.size(); ++i) {
                auto const [numer, denom] = exact_input[i];
                check_equal(xstd::div(numer, denom), expected[i]);
                check_equal(xstd::euclidean_div(numer, denom), expected[i]);
                check_equal(xstd::floored_div(numer, denom), expected[i]);
        }
}


BOOST_AUTO_TEST_CASE(BoundaryDivisions)
{
        using limits = std::numeric_limits<int>;

        check_equal(xstd::div(limits::min(), +1), xstd::div_t{limits::min(), 0});
        check_equal(xstd::euclidean_div(limits::min(), +1), xstd::div_t{limits::min(), 0});
        check_equal(xstd::floored_div(limits::min(), +1), xstd::div_t{limits::min(), 0});

        check_equal(xstd::div(+1, limits::min()), xstd::div_t{0, +1});
        check_equal(xstd::euclidean_div(+1, limits::min()), xstd::div_t{0, +1});
        check_equal(xstd::floored_div(+1, limits::min()), xstd::div_t{-1, limits::min() + 1});

        check_equal(xstd::div(limits::max(), -1), xstd::div_t{-limits::max(), 0});
        check_equal(xstd::euclidean_div(limits::max(), -1), xstd::div_t{-limits::max(), 0});
        check_equal(xstd::floored_div(limits::max(), -1), xstd::div_t{-limits::max(), 0});
}

BOOST_AUTO_TEST_CASE(FlooredDiv)
{
        auto const floored_div = std::vector<xstd::div_t>
        {
                {+2, +2}, {-3, -1}, {-3, +1}, {+2, -2},
                { 0, +1}, {-1, -1}, {-1, +1}, { 0, -1}
        };

        std::vector<xstd::div_t> floored_res;
        std::transform(input.begin(), input.end(), std::back_inserter(floored_res), [](auto const& p) {
                return xstd::floored_div(p.first, p.second);
        });

        check_equal_ranges(floored_res, floored_div);
}

BOOST_AUTO_TEST_CASE(Formatter)
{
        xstd::div_t const d { 1, -2 };
        BOOST_CHECK((xstd::as_tuple(d) == std::tuple(1, -2)));
        BOOST_CHECK_EQUAL(std::format("{}", d), "(1, -2)");
        BOOST_CHECK_EQUAL(std::format(L"{}", d), std::wstring(L"(1, -2)"));
}

BOOST_AUTO_TEST_SUITE_END()
