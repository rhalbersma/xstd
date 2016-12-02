#include <xstd/cstdlib.hpp>                     // truncated_div, floored_div, euclidean_div
#include <boost/range/algorithm/equal.hpp>      // equal
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_CHECK, BOOST_AUTO_TEST_SUITE_END
#include <cstdlib>                              // div_t
#include <utility>                              // pair
#include <vector>                               // vector

BOOST_AUTO_TEST_SUITE(CStdLib)

BOOST_AUTO_TEST_CASE(TruncatedFlooredEuclideanDivision)
{
        // http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

        auto const input = std::vector<std::pair<int, int>>
        {
                {+8, +3}, {+8, -3}, {-8, +3}, {-8, -3},
                {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}
        };

        auto const truncated_result = std::vector<std::div_t>
        {
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        };

        auto const floored_result = std::vector<std::div_t>
        {
                {+2, +2}, {-3, -1}, {-3, +1}, {+2, -2},
                { 0, +1}, {-1, -1}, {-1, +1}, { 0, -1}
        };

        auto const euclidean_result = std::vector<std::div_t>
        {
                {+2, +2}, {-2, +2}, {-3, +1}, {+3, +1},
                { 0, +1}, { 0, +1}, {-1, +1}, {+1, +1}
        };

        using namespace xstd;

        BOOST_CHECK(
                boost::equal(input, truncated_result, [](auto const& lhs, auto const& rhs) {
                        return truncated_div(lhs.first, lhs.second) == rhs;
                })
        );

        BOOST_CHECK(
                boost::equal(input, floored_result, [](auto const& lhs, auto const& rhs) {
                        return floored_div(lhs.first, lhs.second) == rhs;
                })
        );

        BOOST_CHECK(
                boost::equal(input, euclidean_result, [](auto const& lhs, auto const& rhs) {
                        return euclidean_div(lhs.first, lhs.second) == rhs;
                })
        );
}

BOOST_AUTO_TEST_SUITE_END()
