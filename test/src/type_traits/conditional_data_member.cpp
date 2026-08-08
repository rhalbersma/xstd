//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/conditional_data_member.hpp> // conditional_data_member
#include <xstd/test/constexpr.hpp>                      // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <type_traits>                                  // is_empty_v, is_same_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

struct tag1;
struct tag2;

struct compressed_member
{
        [[XSTD_NO_UNIQUE_ADDRESS]] xstd::empty_type<tag1> empty;
        int value;
};

static_assert(sizeof(compressed_member) == sizeof(int));

// the tag can be declared in place, which is how a class names each of its
// conditional members; both stand in for the same Type, the colliding case
using member1 = xstd::conditional_data_member_t<false, tag1, struct member1_tag>;
using member2 = xstd::conditional_data_member_t<false, tag1, struct member2_tag>;

BOOST_AUTO_TEST_CASE(ConditionalDataMember)
{
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::conditional_data_member_t<true, tag1, tag2>, tag1>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::conditional_data_member_t<false, tag1, tag2>, xstd::empty_type<tag2>>));
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<xstd::conditional_data_member_t<false, tag1, tag2>>));

        // the tag names the member rather than the type it stands in for, so
        // two absent ones can still overlap in the layout
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<member1>));
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<member2>));
        XSTD_CONSTEXPR_CHECK((not std::is_same_v<member1, member2>));

        // the tag is inert when the member is present: same Type, same tags
        // as above, and the two agree once the condition holds
        XSTD_CONSTEXPR_CHECK((std::is_same_v<
                              xstd::conditional_data_member_t<true, tag1, struct member1_tag>,
                              xstd::conditional_data_member_t<true, tag1, struct member2_tag>>));

        // an absent member never needs its tag defined
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<xstd::conditional_data_member_t<false, tag1, struct undefined_tag>>));
}

BOOST_AUTO_TEST_SUITE_END()
