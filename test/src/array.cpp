//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/array.hpp>               // array_from_types
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK_EQUAL
#include <array>                        // array
#include <cstddef>                      // size_t
#include <tuple>                        // tuple
#include <type_traits>                  // is_same_v

using namespace xstd;

BOOST_AUTO_TEST_SUITE(Array)

template<class... T>
struct type_list {};

BOOST_AUTO_TEST_CASE(ArrayFromTypes)
{
        constexpr auto sizes = array_from_types<type_list<int, double, char>>()([](auto x) {
                return sizeof(x);
        });

        static_assert(std::is_same_v<decltype(sizes), std::array<std::size_t, 3> const>);
        static_assert(sizes == std::array{sizeof(int), sizeof(double), sizeof(char)});

        BOOST_CHECK_EQUAL(sizes[0], sizeof(int));
        BOOST_CHECK_EQUAL(sizes[1], sizeof(double));
        BOOST_CHECK_EQUAL(sizes[2], sizeof(char));
}

BOOST_AUTO_TEST_CASE(WorksWithAnyTypeList)
{
        // any variadic class template works as the type list, e.g. std::tuple
        auto const ones = array_from_types<std::tuple<bool, char>>()([](auto) {
                return 1;
        });
        BOOST_CHECK_EQUAL(ones.size(), 2u);
        BOOST_CHECK_EQUAL(ones[0], 1);
        BOOST_CHECK_EQUAL(ones[1], 1);
}

BOOST_AUTO_TEST_SUITE_END()
