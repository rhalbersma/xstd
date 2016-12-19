#include <xstd/cstdlib.hpp>     // truncated_div, floored_div, euclidean_div
#include <algorithm>            // equal
#include <cassert>              // assert
#include <cstdlib>              // div_t
#include <utility>              // pair
#include <vector>               // vector

int main()
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

        assert(
                std::equal(input.cbegin(), input.cend(), truncated_result.cbegin(), truncated_result.cend(), [](auto const& lhs, auto const& rhs) {
                        return truncated_div(lhs.first, lhs.second) == rhs;
                })
        );

        assert(
                std::equal(input.cbegin(), input.cend(), floored_result.cbegin(), floored_result.cend(), [](auto const& lhs, auto const& rhs) {
                        return floored_div(lhs.first, lhs.second) == rhs;
                })
        );

        assert(
                std::equal(input.cbegin(), input.cend(), euclidean_result.cbegin(), euclidean_result.cend(), [](auto const& lhs, auto const& rhs) {
                        return euclidean_div(lhs.first, lhs.second) == rhs;
                })
        );
}
