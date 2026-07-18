//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/utility.hpp>         // to_underlying
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <type_traits>              // integral_constant
#include <utility>                  // to_underlying

using namespace xstd;

BOOST_AUTO_TEST_SUITE(Utility)

enum class e1 {};
enum class e2 : bool {};
enum class e3 : char {};
enum class e4 : unsigned char {};
enum class e5 : unsigned {};

template<e1 N> using e1_ = std::integral_constant<e1, N>;
template<e2 N> using e2_ = std::integral_constant<e2, N>;
template<e3 N> using e3_ = std::integral_constant<e3, N>;
template<e4 N> using e4_ = std::integral_constant<e4, N>;
template<e5 N> using e5_ = std::integral_constant<e5, N>;

BOOST_AUTO_TEST_CASE(ToUnderlyingType)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(std::to_underlying(e1()), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(std::to_underlying(e2()), false);
        XSTD_CONSTEXPR_CHECK_EQUAL(std::to_underlying(e3()), static_cast<char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(std::to_underlying(e4()), static_cast<unsigned char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(std::to_underlying(e5()), static_cast<unsigned>(0));

        // use {} instead of () inside <> to avoid vexing parse
        XSTD_CONSTEXPR_CHECK_EQUAL(to_underlying(e1_<e1{}>()), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(to_underlying(e2_<e2{}>()), false);
        XSTD_CONSTEXPR_CHECK_EQUAL(to_underlying(e3_<e3{}>()), static_cast<char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(to_underlying(e4_<e4{}>()), static_cast<unsigned char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(to_underlying(e5_<e5{}>()), static_cast<unsigned>(0));
}

BOOST_AUTO_TEST_CASE(AlignedSize)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(aligned_size(0, 8), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(aligned_size(1, 8), 8);
        XSTD_CONSTEXPR_CHECK_EQUAL(aligned_size(8, 8), 8);
        XSTD_CONSTEXPR_CHECK_EQUAL(aligned_size(9, 8), 16);
        XSTD_CONSTEXPR_CHECK_EQUAL(aligned_size(64, 8), 64);
        XSTD_CONSTEXPR_CHECK_EQUAL(aligned_size(65, 8), 72);
}

BOOST_AUTO_TEST_SUITE_END()
