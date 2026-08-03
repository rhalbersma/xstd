//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/utility.hpp>         // to_underlying
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <type_traits>              // integral_constant
#include <utility>                  // to_underlying

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
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e1_<e1{}>()), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e2_<e2{}>()), false);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e3_<e3{}>()), static_cast<char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e4_<e4{}>()), static_cast<unsigned char>(0));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::to_underlying(e5_<e5{}>()), static_cast<unsigned>(0));
}

template<class T>
concept has_to_underlying = requires (T t) { xstd::to_underlying(t); };

BOOST_AUTO_TEST_CASE(ToUnderlyingTypeIsConstrained)
{
        XSTD_CONSTEXPR_CHECK(has_to_underlying<e1_<e1{}>>);

        // the enum constraint is checked before the return type is
        // formed, so a non-enum argument is a substitution failure rather
        // than an ill-formed underlying_type_t (CWG2369, see design.md)
        XSTD_CONSTEXPR_CHECK((not has_to_underlying<std::integral_constant<int, 0>>));
        XSTD_CONSTEXPR_CHECK(not has_to_underlying<double>);
}

BOOST_AUTO_TEST_SUITE_END()
