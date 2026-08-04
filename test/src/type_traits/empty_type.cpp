//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits/empty_type.hpp> // empty_type
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                         // strong_ordering
#include <type_traits>                     // is_constructible_v, is_convertible_v, is_empty_v, is_nothrow_constructible_v, is_nothrow_default_constructible_v, is_same_v, is_trivially_constructible_v, is_trivially_copyable_v

BOOST_AUTO_TEST_SUITE(TypeTraits)

struct tag1;
struct tag2;

BOOST_AUTO_TEST_CASE(EmptyType)
{
        using empty1 = xstd::empty_type<tag1>;
        using empty2 = xstd::empty_type<tag2>;

        XSTD_CONSTEXPR_CHECK((std::is_empty_v<empty1>));
        XSTD_CONSTEXPR_CHECK((not std::is_same_v<empty1, empty2>));

        // constructible from anything, but only explicitly
        XSTD_CONSTEXPR_CHECK((std::is_constructible_v<empty1, int, double>));
        XSTD_CONSTEXPR_CHECK((not std::is_convertible_v<int, empty1>));

        // the catch-all constructor never hijacks copy/move construction,
        // not even from a non-const lvalue
        XSTD_CONSTEXPR_CHECK(std::is_trivially_copyable_v<empty1>);
        XSTD_CONSTEXPR_CHECK((std::is_trivially_constructible_v<empty1, empty1&>));
        XSTD_CONSTEXPR_CHECK((std::is_trivially_constructible_v<empty1, empty1 const&>));
        XSTD_CONSTEXPR_CHECK((std::is_trivially_constructible_v<empty1, empty1&&>));

        // neither constructor can throw
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_default_constructible_v<empty1>);
        XSTD_CONSTEXPR_CHECK((std::is_nothrow_constructible_v<empty1, int, double>));

        // stateless: all instances compare equal, regardless of construction.
        // operator<=> is a hidden friend, so these also pin that ADL finds it
        // and that it still implies a defaulted operator==
        XSTD_CONSTEXPR_CHECK(empty1(1, 2.0) == empty1());
        XSTD_CONSTEXPR_CHECK(empty1() == empty1(42));
        XSTD_CONSTEXPR_CHECK((empty1() <=> empty1()) == std::strong_ordering::equal);
        XSTD_CONSTEXPR_CHECK(noexcept(empty1() == empty1()));
        XSTD_CONSTEXPR_CHECK(noexcept(empty1() <=> empty1()));
}

BOOST_AUTO_TEST_SUITE_END()
