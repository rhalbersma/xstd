#include <xstd/cstddef.hpp>             // _t, _z, ptrdiff_t, size_t
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE BOOST_CHECK_EQUAL BOOST_AUTO_TEST_SUITE_END

BOOST_AUTO_TEST_SUITE(Suffixes)

using namespace xstd::literals::support_literals;

BOOST_AUTO_TEST_CASE(SizeT)
{
        static_assert(std::is_same<decltype(0_z), std::size_t>::value, "");
        std::size_t a = 0_z;
        auto b = 0_z;
        BOOST_CHECK_EQUAL(a, static_cast<std::size_t>(0));
        BOOST_CHECK_EQUAL(a, b);
}

BOOST_AUTO_TEST_CASE(PtrDiffT)
{
        static_assert(std::is_same<decltype(0_t), ptrdiff_t>::value, "");
        std::size_t a = 0_t;
        auto b = 0_t;
        BOOST_CHECK_EQUAL(a, static_cast<ptrdiff_t>(0));
        BOOST_CHECK_EQUAL(a, b);
}

BOOST_AUTO_TEST_SUITE_END()
