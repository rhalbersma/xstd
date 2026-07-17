//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>             // div, floored_div, euclidean_div
#include <xstd/test/constexpr.hpp>      // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS
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

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE(Abs)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(-2), 2);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(-1), 1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs( 0), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(+1), 1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(+2), 2);

        using limits = std::numeric_limits<int>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::min() + 1), limits::max());

        // abs is a plain <cstdlib>-style overload (no template, no unsigned
        // support): a short argument is promoted to int like any other
        // call, which keeps it well-defined for short's lowest value.
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(std::numeric_limits<short>::min()), -(std::numeric_limits<short>::min() + 1) + 1);
}

BOOST_AUTO_TEST_CASE(Labs)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::labs(-2L), 2L);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::labs(+2L), 2L);

        using limits = std::numeric_limits<long>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::labs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::labs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Llabs)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llabs(-2LL), 2LL);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llabs(+2LL), 2LL);

        using limits = std::numeric_limits<long long>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llabs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llabs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Imaxabs)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxabs(-2), 2);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxabs(+2), 2);

        using limits = std::numeric_limits<std::intmax_t>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxabs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxabs(limits::min() + 1), limits::max());
}

BOOST_AUTO_TEST_CASE(Sign)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(-2), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(-1), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign( 0),  0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(+1), +1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(+2), +1);
}

BOOST_AUTO_TEST_CASE(Lsign)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::lsign(-2L), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::lsign( 0L),  0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::lsign(+2L), +1);
}

BOOST_AUTO_TEST_CASE(Llsign)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llsign(-2LL), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llsign( 0LL),  0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::llsign(+2LL), +1);
}

BOOST_AUTO_TEST_CASE(Imaxsign)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxsign(-2), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxsign( 0),  0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxsign(+2), +1);
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
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+8, +3), (xstd::div_t{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+8, -3), (xstd::div_t{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(-8, +3), (xstd::div_t{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(-8, -3), (xstd::div_t{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+1, +2), (xstd::div_t{ 0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+1, -2), (xstd::div_t{ 0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(-1, +2), (xstd::div_t{ 0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(-1, -2), (xstd::div_t{ 0, -1}));
}

BOOST_AUTO_TEST_CASE(EuclideanDiv)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+8, +3), (xstd::div_t{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+8, -3), (xstd::div_t{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(-8, +3), (xstd::div_t{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(-8, -3), (xstd::div_t{+3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+1, +2), (xstd::div_t{ 0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+1, -2), (xstd::div_t{ 0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(-1, +2), (xstd::div_t{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(-1, -2), (xstd::div_t{+1, +1}));
}

BOOST_AUTO_TEST_CASE(ExactDivisions)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+6, +3), (xstd::div_t{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+6, +3), (xstd::div_t{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+6, +3), (xstd::div_t{+2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+6, -3), (xstd::div_t{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+6, -3), (xstd::div_t{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+6, -3), (xstd::div_t{-2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(-6, +3), (xstd::div_t{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(-6, +3), (xstd::div_t{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(-6, +3), (xstd::div_t{-2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(-6, -3), (xstd::div_t{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(-6, -3), (xstd::div_t{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(-6, -3), (xstd::div_t{+2, 0}));
}

// detail::magnitude/lmagnitude/llmagnitude/imaxmagnitude are only exercised
// transitively above, through the div families' assert() guards; check
// their own MIN-boundary wraparound directly and at compile time, since
// that's the one case a widening-based |x| could not have handled.
BOOST_AUTO_TEST_CASE(Magnitude)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::magnitude(-2), 2u);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::magnitude(+2), 2u);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::magnitude(0), 0u);

        using limits = std::numeric_limits<int>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::magnitude(limits::min()), static_cast<unsigned>(limits::max()) + 1u);

        using llimits = std::numeric_limits<long>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::lmagnitude(llimits::min()), static_cast<unsigned long>(llimits::max()) + 1ul);

        using lllimits = std::numeric_limits<long long>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::llmagnitude(lllimits::min()), static_cast<unsigned long long>(lllimits::max()) + 1ull);

        using imaxlimits = std::numeric_limits<std::intmax_t>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::detail::imaxmagnitude(imaxlimits::min()), static_cast<std::uintmax_t>(imaxlimits::max()) + std::uintmax_t{1});
}

BOOST_AUTO_TEST_CASE(BoundaryDivisions)
{
        using limits = std::numeric_limits<int>;

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(limits::min(), +1), (xstd::div_t{limits::min(), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(limits::min(), +1), (xstd::div_t{limits::min(), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(limits::min(), +1), (xstd::div_t{limits::min(), 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(+1, limits::min()), (xstd::div_t{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(+1, limits::min()), (xstd::div_t{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+1, limits::min()), (xstd::div_t{-1, limits::min() + 1}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(limits::max(), -1), (xstd::div_t{-limits::max(), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(limits::max(), -1), (xstd::div_t{-limits::max(), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(limits::max(), -1), (xstd::div_t{-limits::max(), 0}));
}

BOOST_AUTO_TEST_CASE(FlooredDiv)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+8, +3), (xstd::div_t{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+8, -3), (xstd::div_t{-3, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(-8, +3), (xstd::div_t{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(-8, -3), (xstd::div_t{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+1, +2), (xstd::div_t{ 0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(+1, -2), (xstd::div_t{-1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(-1, +2), (xstd::div_t{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(-1, -2), (xstd::div_t{ 0, -1}));
}

// ldiv/lldiv/imaxdiv (and their euclidean_/floored_ counterparts) run the
// exact same algorithm as div, just parameterized by width; (-8, +3) and
// each type's MIN boundary are enough to confirm the width plumbing itself,
// since correctness of the algorithm is already covered above for int.
BOOST_AUTO_TEST_CASE(Ldiv)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::ldiv(-8L, +3L), (xstd::ldiv_t{-2L, -2L}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_ldiv(-8L, +3L), (xstd::ldiv_t{-3L, +1L}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_ldiv(-8L, +3L), (xstd::ldiv_t{-3L, +1L}));

        // Negative denom alongside a negative remainder: the only input
        // shape not already hit above, needed to cover euclidean_ldiv's
        // (denom > 0 ? ... : ...) branch.
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::ldiv(-8L, -3L), (xstd::ldiv_t{2L, -2L}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_ldiv(-8L, -3L), (xstd::ldiv_t{3L, 1L}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_ldiv(-8L, -3L), (xstd::ldiv_t{2L, -2L}));

        using limits = std::numeric_limits<long>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::ldiv(limits::min(), +1L), (xstd::ldiv_t{limits::min(), 0L}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_ldiv(limits::min(), +1L), (xstd::ldiv_t{limits::min(), 0L}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_ldiv(limits::min(), +1L), (xstd::ldiv_t{limits::min(), 0L}));
}

BOOST_AUTO_TEST_CASE(Lldiv)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::lldiv(-8LL, +3LL), (xstd::lldiv_t{-2LL, -2LL}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_lldiv(-8LL, +3LL), (xstd::lldiv_t{-3LL, +1LL}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_lldiv(-8LL, +3LL), (xstd::lldiv_t{-3LL, +1LL}));

        // Negative denom alongside a negative remainder: the only input
        // shape not already hit above, needed to cover euclidean_lldiv's
        // (denom > 0 ? ... : ...) branch.
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::lldiv(-8LL, -3LL), (xstd::lldiv_t{2LL, -2LL}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_lldiv(-8LL, -3LL), (xstd::lldiv_t{3LL, 1LL}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_lldiv(-8LL, -3LL), (xstd::lldiv_t{2LL, -2LL}));

        using limits = std::numeric_limits<long long>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::lldiv(limits::min(), +1LL), (xstd::lldiv_t{limits::min(), 0LL}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_lldiv(limits::min(), +1LL), (xstd::lldiv_t{limits::min(), 0LL}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_lldiv(limits::min(), +1LL), (xstd::lldiv_t{limits::min(), 0LL}));
}

BOOST_AUTO_TEST_CASE(Imaxdiv)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxdiv(-8, +3), (xstd::imaxdiv_t{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_imaxdiv(-8, +3), (xstd::imaxdiv_t{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_imaxdiv(-8, +3), (xstd::imaxdiv_t{-3, +1}));

        // Negative denom alongside a negative remainder: the only input
        // shape not already hit above, needed to cover euclidean_imaxdiv's
        // (denom > 0 ? ... : ...) branch.
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxdiv(-8, -3), (xstd::imaxdiv_t{2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_imaxdiv(-8, -3), (xstd::imaxdiv_t{3, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_imaxdiv(-8, -3), (xstd::imaxdiv_t{2, -2}));

        using limits = std::numeric_limits<std::intmax_t>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::imaxdiv(limits::min(), 1), (xstd::imaxdiv_t{limits::min(), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_imaxdiv(limits::min(), 1), (xstd::imaxdiv_t{limits::min(), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_imaxdiv(limits::min(), 1), (xstd::imaxdiv_t{limits::min(), 0}));
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
