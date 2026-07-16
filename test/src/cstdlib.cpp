//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>             // div, floored_div, euclidean_div
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS
#include <algorithm>                    // transform
#include <array>                        // array
#include <cstdlib>                      // div, div_t
#include <format>                       // format
#include <iterator>                     // back_inserter
#include <limits>                       // numeric_limits
#include <sstream>                      // istringstream, ostringstream, stringstream, wostringstream
#include <string>                       // string
#include <utility>                      // pair
#include <vector>                       // vector

// The whole point of these functions (p0533-style) is being usable in
// constant expressions; verify that at compile time.
static_assert(xstd::abs(-2) == 2);
static_assert(xstd::sign(-2) == -1);
static_assert(xstd::div(+8, +3) == xstd::div_t{+2, +2});
static_assert(xstd::euclidean_div(-8, +3) == xstd::div_t{-3, +1});
static_assert(xstd::floored_div(-8, +3) == xstd::div_t{-3, +1});

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

        BOOST_CHECK_EQUAL_COLLECTIONS(
                std_res.begin(), std_res.end(),
                std_div.begin(), std_div.end()
        );
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

        BOOST_CHECK_EQUAL_COLLECTIONS(
                truncated_res.begin(), truncated_res.end(),
                div.begin(), div.end()
        );
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

        BOOST_CHECK_EQUAL_COLLECTIONS(
                euclidean_res.begin(), euclidean_res.end(),
                euclidean_div.begin(), euclidean_div.end()
        );
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
                BOOST_CHECK_EQUAL(xstd::div(numer, denom), expected[i]);
                BOOST_CHECK_EQUAL(xstd::euclidean_div(numer, denom), expected[i]);
                BOOST_CHECK_EQUAL(xstd::floored_div(numer, denom), expected[i]);
        }
}


BOOST_AUTO_TEST_CASE(BoundaryDivisions)
{
        using limits = std::numeric_limits<int>;

        BOOST_CHECK_EQUAL(xstd::div(limits::min(), +1), xstd::div_t{limits::min(), 0});
        BOOST_CHECK_EQUAL(xstd::euclidean_div(limits::min(), +1), xstd::div_t{limits::min(), 0});
        BOOST_CHECK_EQUAL(xstd::floored_div(limits::min(), +1), xstd::div_t{limits::min(), 0});

        BOOST_CHECK_EQUAL(xstd::div(+1, limits::min()), xstd::div_t{0, +1});
        BOOST_CHECK_EQUAL(xstd::euclidean_div(+1, limits::min()), xstd::div_t{0, +1});
        BOOST_CHECK_EQUAL(xstd::floored_div(+1, limits::min()), xstd::div_t{-1, limits::min() + 1});

        BOOST_CHECK_EQUAL(xstd::div(limits::max(), -1), xstd::div_t{-limits::max(), 0});
        BOOST_CHECK_EQUAL(xstd::euclidean_div(limits::max(), -1), xstd::div_t{-limits::max(), 0});
        BOOST_CHECK_EQUAL(xstd::floored_div(limits::max(), -1), xstd::div_t{-limits::max(), 0});
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

        BOOST_CHECK_EQUAL_COLLECTIONS(
                floored_res.begin(), floored_res.end(),
                floored_div.begin(), floored_div.end()
        );
}

BOOST_AUTO_TEST_CASE(Formatter)
{
        xstd::div_t const d { 1, -2 };
        BOOST_CHECK_EQUAL(std::format("{}", d), "(1, -2)");
        BOOST_CHECK_EQUAL(std::format(L"{}", d), std::wstring(L"(1, -2)"));
}

BOOST_AUTO_TEST_CASE(StreamInsertion)
{
        xstd::div_t const d { 1, -2 };

        std::ostringstream oss;
        oss << d;
        BOOST_CHECK_EQUAL(oss.str(), "(1, -2)");
        BOOST_CHECK_EQUAL(oss.str(), std::format("{}", d));

        std::wostringstream woss;
        woss << d;
        BOOST_CHECK(woss.str() == L"(1, -2)");
        BOOST_CHECK(woss.str() == std::format(L"{}", d));
}

BOOST_AUTO_TEST_SUITE_END()
