#include <xstd/cstddef.hpp>             // _t, _z, ptrdiff_t, size_t
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE BOOST_AUTO_TEST_SUITE_END
#include <numeric>                      // accumulate

BOOST_AUTO_TEST_SUITE(Suffixes)

BOOST_AUTO_TEST_CASE(AutoInitializedTypeIsDeducedFromLiteral)
{
        using namespace xstd::literals::support_literals;
        auto const i = 0_t;
        auto const j = 0_z;
        static_assert(std::is_same<decltype(i), std::ptrdiff_t>::value, "");
        static_assert(std::is_same<decltype(j), std::size_t>::value, "");
}

BOOST_AUTO_TEST_CASE(AccumulateReturnTypeIsDeducedFromLiteral)
{
        using namespace xstd::literals::support_literals;
        int const a[] = { 1, 2 };
        auto const i = std::accumulate(std::begin(a), std::end(a), 0_t);
        auto const j = std::accumulate(std::begin(a), std::end(a), 0_z);
        static_assert(std::is_same<decltype(i), std::ptrdiff_t>::value, "");
        static_assert(std::is_same<decltype(j), std::size_t>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()