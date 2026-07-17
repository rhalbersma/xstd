//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>             // div, floored_div, euclidean_div
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_MESSAGE
#include <algorithm>                    // transform
#include <array>                        // array
#include <cstdint>                      // intmax_t
#include <cstdlib>                      // div, div_t
#include <format>                       // format
#include <iterator>                     // back_inserter
#include <limits>                       // numeric_limits
#include <sstream>                      // ostringstream
#include <utility>                      // pair
#include <vector>                       // vector

// The whole point of these functions (p0533-style) is being usable in
// constant expressions; verify that at compile time.
static_assert(xstd::abs(-2) == 2);
static_assert(xstd::labs(-2L) == 2L);
static_assert(xstd::llabs(-2LL) == 2LL);
static_assert(xstd::imaxabs(std::intmax_t{-2}) == 2);
static_assert(xstd::sign(-2) == -1);
static_assert(xstd::lsign(-2L) == -1);
static_assert(xstd::llsign(-2LL) == -1);
static_assert(xstd::imaxsign(std::intmax_t{-2}) == -1);
static_assert(xstd::div(+8, +3) == xstd::div_t{+2, +2});
static_assert(xstd::ldiv(+8L, +3L) == xstd::ldiv_t{+2L, +2L});
static_assert(xstd::lldiv(+8LL, +3LL) == xstd::lldiv_t{+2LL, +2LL});
static_assert(xstd::imaxdiv(std::intmax_t{+8}, std::intmax_t{+3}) == xstd::imaxdiv_t{+2, +2});
static_assert(xstd::euclidean_div(-8, +3) == xstd::div_t{-3, +1});
static_assert(xstd::euclidean_ldiv(-8L, +3L) == xstd::ldiv_t{-3L, +1L});
static_assert(xstd::euclidean_lldiv(-8LL, +3LL) == xstd::lldiv_t{-3LL, +1LL});
static_assert(xstd::euclidean_imaxdiv(std::intmax_t{-8}, std::intmax_t{+3}) == xstd::imaxdiv_t{-3, +1});
static_assert(xstd::floored_div(-8, +3) == xstd::div_t{-3, +1});
static_assert(xstd::floored_ldiv(-8L, +3L) == xstd::ldiv_t{-3L, +1L});
static_assert(xstd::floored_lldiv(-8LL, +3LL) == xstd::lldiv_t{-3LL, +1LL});
static_assert(xstd::floored_imaxdiv(std::intmax_t{-8}, std::intmax_t{+3}) == xstd::imaxdiv_t{-3, +1});

BOOST_AUTO_TEST_SUITE(CStdLib)

// abs/labs/llabs/sign are plain <cstdlib>-style overloads (no templates, no
// unsigned support): a short argument is promoted to int like any other
// call, which keeps abs well-defined for its lowest value.
static_assert(xstd::abs(std::numeric_limits<short>::min()) == -(std::numeric_limits<short>::min() + 1) + 1);

BOOST_AUTO_TEST_CASE(Abs)
{
        BOOST_CHECK_EQUAL(xstd::abs(-2), 2);
        BOOST_CHECK_EQUAL(xstd::abs(-1), 1);
        BOOST_CHECK_EQUAL(xstd::abs( 0), 0);
        BOOST_CHECK_EQUAL(xstd::abs(+1), 1);
        BOOST_CHECK_EQUAL(xstd::abs(+2), 2);

        using limits = std::numeric_limits<int>;
        BOOST_CHECK_EQUAL(xstd::abs(limits::max()), limits::max());
        BOOST_CHECK_EQUAL(xstd::abs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Labs)
{
        BOOST_CHECK_EQUAL(xstd::labs(-2L), 2L);
        BOOST_CHECK_EQUAL(xstd::labs(+2L), 2L);

        using limits = std::numeric_limits<long>;
        BOOST_CHECK_EQUAL(xstd::labs(limits::max()), limits::max());
        BOOST_CHECK_EQUAL(xstd::labs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Llabs)
{
        BOOST_CHECK_EQUAL(xstd::llabs(-2LL), 2LL);
        BOOST_CHECK_EQUAL(xstd::llabs(+2LL), 2LL);

        using limits = std::numeric_limits<long long>;
        BOOST_CHECK_EQUAL(xstd::llabs(limits::max()), limits::max());
        BOOST_CHECK_EQUAL(xstd::llabs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Imaxabs)
{
        BOOST_CHECK_EQUAL(xstd::imaxabs(-2), 2);
        BOOST_CHECK_EQUAL(xstd::imaxabs(+2), 2);

        using limits = std::numeric_limits<std::intmax_t>;
        BOOST_CHECK_EQUAL(xstd::imaxabs(limits::max()), limits::max());
        BOOST_CHECK_EQUAL(xstd::imaxabs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Sign)
{
        BOOST_CHECK_EQUAL(xstd::sign(-2), -1);
        BOOST_CHECK_EQUAL(xstd::sign(-1), -1);
        BOOST_CHECK_EQUAL(xstd::sign( 0),  0);
        BOOST_CHECK_EQUAL(xstd::sign(+1), +1);
        BOOST_CHECK_EQUAL(xstd::sign(+2), +1);
}

BOOST_AUTO_TEST_CASE(Lsign)
{
        BOOST_CHECK_EQUAL(xstd::lsign(-2L), -1);
        BOOST_CHECK_EQUAL(xstd::lsign( 0L),  0);
        BOOST_CHECK_EQUAL(xstd::lsign(+2L), +1);
}

BOOST_AUTO_TEST_CASE(Llsign)
{
        BOOST_CHECK_EQUAL(xstd::llsign(-2LL), -1);
        BOOST_CHECK_EQUAL(xstd::llsign( 0LL),  0);
        BOOST_CHECK_EQUAL(xstd::llsign(+2LL), +1);
}

BOOST_AUTO_TEST_CASE(Imaxsign)
{
        BOOST_CHECK_EQUAL(xstd::imaxsign(-2), -1);
        BOOST_CHECK_EQUAL(xstd::imaxsign( 0),  0);
        BOOST_CHECK_EQUAL(xstd::imaxsign(+2), +1);
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

        BOOST_CHECK_EQUAL(xstd::div(limits::min(), +1), (xstd::div_t{limits::min(), 0}));
        BOOST_CHECK_EQUAL(xstd::euclidean_div(limits::min(), +1), (xstd::div_t{limits::min(), 0}));
        BOOST_CHECK_EQUAL(xstd::floored_div(limits::min(), +1), (xstd::div_t{limits::min(), 0}));

        BOOST_CHECK_EQUAL(xstd::div(+1, limits::min()), (xstd::div_t{0, +1}));
        BOOST_CHECK_EQUAL(xstd::euclidean_div(+1, limits::min()), (xstd::div_t{0, +1}));
        BOOST_CHECK_EQUAL(xstd::floored_div(+1, limits::min()), (xstd::div_t{-1, limits::min() + 1}));

        BOOST_CHECK_EQUAL(xstd::div(limits::max(), -1), (xstd::div_t{-limits::max(), 0}));
        BOOST_CHECK_EQUAL(xstd::euclidean_div(limits::max(), -1), (xstd::div_t{-limits::max(), 0}));
        BOOST_CHECK_EQUAL(xstd::floored_div(limits::max(), -1), (xstd::div_t{-limits::max(), 0}));
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

// ldiv/lldiv/imaxdiv (and their euclidean_/floored_ counterparts) run the
// exact same algorithm as div, just parameterized by width; (-8, +3) and
// each type's MIN boundary are enough to confirm the width plumbing itself,
// since correctness of the algorithm is already covered above for int.
BOOST_AUTO_TEST_CASE(Ldiv)
{
        BOOST_CHECK_EQUAL(xstd::ldiv(-8L, +3L), (xstd::ldiv_t{-2L, -2L}));
        BOOST_CHECK_EQUAL(xstd::euclidean_ldiv(-8L, +3L), (xstd::ldiv_t{-3L, +1L}));
        BOOST_CHECK_EQUAL(xstd::floored_ldiv(-8L, +3L), (xstd::ldiv_t{-3L, +1L}));

        using limits = std::numeric_limits<long>;
        BOOST_CHECK_EQUAL(xstd::ldiv(limits::min(), +1L), (xstd::ldiv_t{limits::min(), 0L}));
        BOOST_CHECK_EQUAL(xstd::euclidean_ldiv(limits::min(), +1L), (xstd::ldiv_t{limits::min(), 0L}));
        BOOST_CHECK_EQUAL(xstd::floored_ldiv(limits::min(), +1L), (xstd::ldiv_t{limits::min(), 0L}));
}

BOOST_AUTO_TEST_CASE(Lldiv)
{
        BOOST_CHECK_EQUAL(xstd::lldiv(-8LL, +3LL), (xstd::lldiv_t{-2LL, -2LL}));
        BOOST_CHECK_EQUAL(xstd::euclidean_lldiv(-8LL, +3LL), (xstd::lldiv_t{-3LL, +1LL}));
        BOOST_CHECK_EQUAL(xstd::floored_lldiv(-8LL, +3LL), (xstd::lldiv_t{-3LL, +1LL}));

        using limits = std::numeric_limits<long long>;
        BOOST_CHECK_EQUAL(xstd::lldiv(limits::min(), +1LL), (xstd::lldiv_t{limits::min(), 0LL}));
        BOOST_CHECK_EQUAL(xstd::euclidean_lldiv(limits::min(), +1LL), (xstd::lldiv_t{limits::min(), 0LL}));
        BOOST_CHECK_EQUAL(xstd::floored_lldiv(limits::min(), +1LL), (xstd::lldiv_t{limits::min(), 0LL}));
}

BOOST_AUTO_TEST_CASE(Imaxdiv)
{
        BOOST_CHECK_EQUAL(xstd::imaxdiv(-8, +3), (xstd::imaxdiv_t{-2, -2}));
        BOOST_CHECK_EQUAL(xstd::euclidean_imaxdiv(-8, +3), (xstd::imaxdiv_t{-3, +1}));
        BOOST_CHECK_EQUAL(xstd::floored_imaxdiv(-8, +3), (xstd::imaxdiv_t{-3, +1}));

        using limits = std::numeric_limits<std::intmax_t>;
        BOOST_CHECK_EQUAL(xstd::imaxdiv(limits::min(), 1), (xstd::imaxdiv_t{limits::min(), 0}));
        BOOST_CHECK_EQUAL(xstd::euclidean_imaxdiv(limits::min(), 1), (xstd::imaxdiv_t{limits::min(), 0}));
        BOOST_CHECK_EQUAL(xstd::floored_imaxdiv(limits::min(), 1), (xstd::imaxdiv_t{limits::min(), 0}));
}

BOOST_AUTO_TEST_CASE(Formatter)
{
        BOOST_CHECK_EQUAL(std::format("{}", xstd::div_t{ 1, -2 }), "(1, -2)");
        BOOST_CHECK_EQUAL(std::format("{}", xstd::ldiv_t{ 1L, -2L }), "(1, -2)");
        BOOST_CHECK_EQUAL(std::format("{}", xstd::lldiv_t{ 1LL, -2LL }), "(1, -2)");
        BOOST_CHECK_EQUAL(std::format("{}", xstd::imaxdiv_t{ 1, -2 }), "(1, -2)");
}

BOOST_AUTO_TEST_CASE(StreamInsertion)
{
        std::ostringstream oss;
        oss << xstd::div_t{ 1, -2 };
        BOOST_CHECK_EQUAL(oss.str(), "(1, -2)");

        std::ostringstream loss;
        loss << xstd::ldiv_t{ 1L, -2L };
        BOOST_CHECK_EQUAL(loss.str(), "(1, -2)");

        std::ostringstream lloss;
        lloss << xstd::lldiv_t{ 1LL, -2LL };
        BOOST_CHECK_EQUAL(lloss.str(), "(1, -2)");

        std::ostringstream imaxoss;
        imaxoss << xstd::imaxdiv_t{ 1, -2 };
        BOOST_CHECK_EQUAL(imaxoss.str(), "(1, -2)");
}

BOOST_AUTO_TEST_SUITE_END()
