//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>             // truncated_div, floored_div, euclidean_div
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE_END
#include <algorithm>                    // transform
#include <array>                        // array
#include <cstdlib>                      // div, div_t
#include <iosfwd>                       // basic_ostream
#include <utility>                      // pair

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE(IsPowerOf2)
{
        BOOST_CHECK(xstd::is_power_of_2(1));
        BOOST_CHECK(xstd::is_power_of_2(2));
        BOOST_CHECK(xstd::is_power_of_2(4));
        BOOST_CHECK(xstd::is_power_of_2(8));

        BOOST_CHECK(not xstd::is_power_of_2(0));
        BOOST_CHECK(not xstd::is_power_of_2(3));
        BOOST_CHECK(not xstd::is_power_of_2(5));
        BOOST_CHECK(not xstd::is_power_of_2(6));
        BOOST_CHECK(not xstd::is_power_of_2(~0u));
}

BOOST_AUTO_TEST_CASE(AlignOn)
{
        constexpr auto N = 1LL << 10;
        for (std::size_t address = 0; address < N; ++address) {
                for (std::size_t alignment = 1; alignment < N; alignment *= 2) {
                        BOOST_CHECK_EQUAL(xstd::align_on(address, alignment) % alignment, 0);
                }
        }
}

BOOST_AUTO_TEST_CASE(Abs)
{
        BOOST_CHECK_EQUAL(xstd::abs(-2), 2);
        BOOST_CHECK_EQUAL(xstd::abs(-1), 1);
        BOOST_CHECK_EQUAL(xstd::abs( 0), 0);
        BOOST_CHECK_EQUAL(xstd::abs(+1), 1);
        BOOST_CHECK_EQUAL(xstd::abs(+2), 2);
}

BOOST_AUTO_TEST_CASE(Signum)
{
        BOOST_CHECK_EQUAL(xstd::signum(-2), -1);
        BOOST_CHECK_EQUAL(xstd::signum(-1), -1);
        BOOST_CHECK_EQUAL(xstd::signum( 0),  0);
        BOOST_CHECK_EQUAL(xstd::signum(+1),  1);
        BOOST_CHECK_EQUAL(xstd::signum(+2),  1);
}

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

auto const input = std::array<std::pair<int, int>, 8>
{{
        {+8, +3}, {+8, -3}, {-8, +3}, {-8, -3},
        {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}
}};

BOOST_AUTO_TEST_CASE(StdDiv)
{
        auto const std_div = std::array<xstd::div_t, 8>
        {{
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        }};

        std::array<xstd::div_t, 8> std_res{};
        std::transform(input.cbegin(), input.cend(), std_res.begin(), [](auto const& p) -> xstd::div_t {
                auto const d = std::div(p.first, p.second);
                return { d.quot, d.rem };
        });

        BOOST_CHECK_EQUAL_COLLECTIONS(
                std_res.cbegin(), std_res.cend(),
                std_div.cbegin(), std_div.cend()
        );
}

BOOST_AUTO_TEST_CASE(TruncatedDiv)
{
        auto const truncated_div = std::array<xstd::div_t, 8>
        {{
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        }};

        std::array<xstd::div_t, 8> truncated_res{};
        std::transform(input.cbegin(), input.cend(), truncated_res.begin(), [](auto const& p) {
                return xstd::truncated_div(p.first, p.second);
        });

        BOOST_CHECK_EQUAL_COLLECTIONS(
                truncated_res.cbegin(), truncated_res.cend(),
                truncated_div.cbegin(), truncated_div.cend()
        );
}

BOOST_AUTO_TEST_CASE(FlooredDiv)
{
        auto const floored_div = std::array<xstd::div_t, 8>
        {{
                {+2, +2}, {-3, -1}, {-3, +1}, {+2, -2},
                { 0, +1}, {-1, -1}, {-1, +1}, { 0, -1}
        }};

        std::array<xstd::div_t, 8> floored_res{};
        std::transform(input.cbegin(), input.cend(), floored_res.begin(), [](auto const& p) {
                return xstd::floored_div(p.first, p.second);
        });

        BOOST_CHECK_EQUAL_COLLECTIONS(
                floored_res.cbegin(), floored_res.cend(),
                floored_div.cbegin(), floored_div.cend()
        );
}

BOOST_AUTO_TEST_CASE(EuclideanDiv)
{
        auto const euclidean_div = std::array<xstd::div_t, 8>
        {{
                {+2, +2}, {-2, +2}, {-3, +1}, {+3, +1},
                { 0, +1}, { 0, +1}, {-1, +1}, {+1, +1}
        }};

        std::array<xstd::div_t, 8> euclidean_res{};
        std::transform(input.cbegin(), input.cend(), euclidean_res.begin(), [](auto const& p) {
                return xstd::euclidean_div(p.first, p.second);
        });

        BOOST_CHECK_EQUAL_COLLECTIONS(
                euclidean_res.cbegin(), euclidean_res.cend(),
                euclidean_div.cbegin(), euclidean_div.cend()
        );
}

BOOST_AUTO_TEST_CASE(test1, * boost::unit_test::expected_failures(1))
{
        BOOST_CHECK_EQUAL((xstd::div_t{0, 1}), (xstd::div_t{1, 0}));
}

BOOST_AUTO_TEST_SUITE_END()
