#include <xstd/cstddef.hpp>             // _zu, size_t
#include <boost/test/included/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <type_traits>                  // is_same

BOOST_AUTO_TEST_SUITE(CStdDef)

BOOST_AUTO_TEST_CASE(UserDefinedLiteralSizeT)
{
        using namespace xstd::support_literals;
        static_assert(std::is_same<decltype(0_zu), std::size_t>{}, "");
        static_assert(0_zu == static_cast<std::size_t>(0), "");

        // repeat previous static_assert at runtime for code coverage purposes
        BOOST_CHECK_EQUAL(0_zu, static_cast<std::size_t>(0));
}

BOOST_AUTO_TEST_SUITE_END()
