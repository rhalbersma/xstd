#include <xstd/cstdlib.hpp>             // truncated_div, floored_div, euclidean_div
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE_END
#include <algorithm>                    // transform
#include <cstdlib>                      // div
#include <iosfwd>                       // basic_ostream
#include <iterator>                     // back_inserter
#include <utility>                      // pair
#include <vector>                       // vector

namespace xstd {

// BOOST_CHECK_EQUAL_COLLECTIONS requires streamable types
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& ostr, div_t const& d)
{
        ostr << "[" << d.quot << "," << d.rem << "]";
        return ostr;
}

}       // namespace xstd

BOOST_AUTO_TEST_SUITE(CStdLib)

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

auto const input = std::vector<std::pair<int, int>>
{
        {+8, +3}, {+8, -3}, {-8, +3}, {-8, -3},
        {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}
};

BOOST_AUTO_TEST_CASE(StdDiv)
{
        // arrange
        auto const truncated_result = std::vector<xstd::div_t>
        {
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        };

        // act
        std::vector<xstd::div_t> std_div;
        std::transform(input.cbegin(), input.cend(), std::back_inserter(std_div), [](auto const& p) -> xstd::div_t {
                auto const d = std::div(p.first, p.second);
                return { d.quot, d.rem };
        });

        // assert
        BOOST_CHECK_EQUAL_COLLECTIONS(std_div.cbegin(), std_div.cend(), truncated_result.cbegin(), truncated_result.cend());
}

BOOST_AUTO_TEST_CASE(TruncatedDiv)
{
        // arrange
        auto const truncated_result = std::vector<xstd::div_t>
        {
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        };

        // act
        std::vector<xstd::div_t> truncated;
        std::transform(input.cbegin(), input.cend(), std::back_inserter(truncated), [](auto const& p) {
                return xstd::truncated_div(p.first, p.second);
        });

        // assert
        BOOST_CHECK_EQUAL_COLLECTIONS(truncated.cbegin(), truncated.cend(), truncated_result.cbegin(), truncated_result.cend());
}

BOOST_AUTO_TEST_CASE(FlooredDiv)
{
        // arrange
        auto const floored_result = std::vector<xstd::div_t>
        {
                {+2, +2}, {-3, -1}, {-3, +1}, {+2, -2},
                { 0, +1}, {-1, -1}, {-1, +1}, { 0, -1}
        };

        // act
        std::vector<xstd::div_t> floored;
        std::transform(input.cbegin(), input.cend(), std::back_inserter(floored), [](auto const& p) {
                return xstd::floored_div(p.first, p.second);
        });

        // assert
        BOOST_CHECK_EQUAL_COLLECTIONS(floored.cbegin(), floored.cend(), floored_result.cbegin(), floored_result.cend());
}

BOOST_AUTO_TEST_CASE(EuclideanDiv)
{
        // arrange
        auto const euclidean_result = std::vector<xstd::div_t>
        {
                {+2, +2}, {-2, +2}, {-3, +1}, {+3, +1},
                { 0, +1}, { 0, +1}, {-1, +1}, {+1, +1}
        };

        // act
        std::vector<xstd::div_t> euclidean;
        std::transform(input.cbegin(), input.cend(), std::back_inserter(euclidean), [](auto const& p) {
                return xstd::euclidean_div(p.first, p.second);
        });

        // assert
        BOOST_CHECK_EQUAL_COLLECTIONS(euclidean.cbegin(), euclidean.cend(), euclidean_result.cbegin(), euclidean_result.cend());
}

BOOST_AUTO_TEST_SUITE_END()
