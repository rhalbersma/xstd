//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdint.hpp>         // int128_t
#include <xstd/cstdlib/div_t.hpp>   // div_t
#include <xstd/ostream/div_t.hpp>   // IWYU pragma: keep; operator<<(ostream, div_t)
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <cstdint>                  // exact-width integer types
#include <sstream>                  // ostringstream
#include <tuple>                    // tuple

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t, xstd::int128_t>;

#ifdef __cpp_lib_format_ranges

BOOST_AUTO_TEST_CASE_TEMPLATE(StreamInsertion, T, exact_width_types)
{
        std::ostringstream oss;
        oss << xstd::div_t<T>{1, -2};
        BOOST_CHECK_EQUAL(oss.str(), "(1, -2)");
}

#else

BOOST_AUTO_TEST_CASE(TupleFormatterUnavailable)
{
        BOOST_CHECK(true);
}

#endif
