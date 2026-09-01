//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/core/type_traits.hpp> // XSTD_NO_UNIQUE_ADDRESS, conditional_data_member_t, empty_type, is_specialization_of
#include <boost/test/unit_test.hpp>  // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                  // same_as
#include <type_traits>               // is_empty_v

BOOST_AUTO_TEST_SUITE(Core)
BOOST_AUTO_TEST_SUITE(TypeTraits)

// What the four are for together: a member that is there or is not, and costs nothing when it is not.
template<bool Present>
struct maybe
{
        [[XSTD_NO_UNIQUE_ADDRESS]] xstd::conditional_data_member_t<Present, int, struct slot> member;
        int value;
};

BOOST_AUTO_TEST_CASE(TheStorageUtilitiesComposeIntoAnOptionalMember)
{
        static_assert(sizeof(maybe<false>) == sizeof(int));
        static_assert(sizeof(maybe<true>) == 2 * sizeof(int));
        static_assert(std::same_as<decltype(maybe<true>::member), int>);
        BOOST_CHECK(true);
}

// The absent branch is an empty_type, which is the one the trait recognises: the two agree on it.
BOOST_AUTO_TEST_CASE(TheAbsentBranchIsAnEmptyTypeTheTraitRecognises)
{
        using absent = xstd::conditional_data_member_t<false, int, struct slot>;

        static_assert(std::is_empty_v<absent>);
        static_assert(std::same_as<absent, xstd::empty_type<struct slot>>);
        static_assert(xstd::is_specialization_of<absent, xstd::empty_type>::value);
        static_assert(not xstd::is_specialization_of<int, xstd::empty_type>::value);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
