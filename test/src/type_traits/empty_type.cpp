//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/empty_type.hpp> // empty_type
#include <xstd/test/constexpr_check.hpp>   // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                         // strong_ordering
#include <concepts>                        // regular, same_as, totally_ordered
#include <type_traits>                     // is_constructible_v, is_convertible_v, is_empty_v, is_nothrow_constructible_v, is_nothrow_default_constructible_v, is_trivially_constructible_v, is_trivially_copyable_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(EmptyTypeIsEmpty)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK((std::is_empty_v<empty>));
}

BOOST_AUTO_TEST_CASE(EmptyTypeIsRegular)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK((std::regular<empty>));
}

BOOST_AUTO_TEST_CASE(EmptyTypeIsTotallyOrdered)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK((std::totally_ordered<empty>));
}

BOOST_AUTO_TEST_CASE(EmptyTypeIsTrivial)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK(std::is_trivially_destructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_default_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_copy_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_copy_assignable_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_move_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_trivially_move_assignable_v<empty>);
}

BOOST_AUTO_TEST_CASE(EmptyTypeIsNoThrow)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK(std::is_nothrow_destructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_default_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_copy_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_copy_assignable_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_move_constructible_v<empty>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_move_assignable_v<empty>);

        // The comparisons too, which nothing above reaches.
        XSTD_CONSTEXPR_CHECK(noexcept(empty{} <=> empty{}));
        XSTD_CONSTEXPR_CHECK(noexcept(empty{} == empty{}));
}

// Defaulted on first declaration is implicitly constexpr, which is the point here.
BOOST_AUTO_TEST_CASE(EmptyTypeIsUsableInConstantExpressions)
{
        using empty = xstd::empty_type<struct tag>;

        static_assert((empty{} <=> empty{}) == std::strong_ordering::equal);
        static_assert(empty{} == empty{});

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

BOOST_AUTO_TEST_CASE(EmptyInstancesAreEqual)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK(empty() == empty(0));
        XSTD_CONSTEXPR_CHECK(empty(0) == empty(1));
}

BOOST_AUTO_TEST_CASE(EmptyInstancesAreEquallyOrdered)
{
        using empty = xstd::empty_type<struct tag>;

        XSTD_CONSTEXPR_CHECK((empty() <=> empty(0)) == std::strong_ordering::equal);
        XSTD_CONSTEXPR_CHECK((empty(0) <=> empty(1)) == std::strong_ordering::equal);
}

BOOST_AUTO_TEST_CASE(EmptyTypesAreNotEqual)
{
        using empty1 = xstd::empty_type<struct tag1>;
        using empty2 = xstd::empty_type<struct tag2>;

        XSTD_CONSTEXPR_CHECK((not std::same_as<empty1, empty2>));
}

BOOST_AUTO_TEST_SUITE_END()
