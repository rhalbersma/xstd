//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/is_signed_like.hpp> // is_signed_like
#include <xstd/test/constexpr.hpp>             // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <type_traits>                         // is_signed_v, is_unsigned_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

// An incomplete class type is answered, not hard-errored: the traits being
// widened cope with one, so these have to as well.
BOOST_AUTO_TEST_CASE(AnswersForIncompleteTypes)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_signed_like_v<struct never_defined>);
}

BOOST_AUTO_TEST_CASE(AgreesWithStd)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<int> == std::is_signed_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<unsigned> == std::is_signed_v<unsigned>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<double> == std::is_signed_v<double>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<void> == std::is_signed_v<void>);
}

BOOST_AUTO_TEST_SUITE_END()
