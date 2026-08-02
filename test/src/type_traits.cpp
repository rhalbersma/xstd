//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>     // is_specialization_of, is_integral_constant, empty_type, conditional_data_member_t
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                  // strong_ordering
#include <complex>                  // complex
#include <type_traits>              // false_type, integral_constant, is_constructible_v, is_convertible_v, is_empty_v, is_nothrow_constructible_v, is_nothrow_default_constructible_v, is_same_v, is_trivially_constructible_v, is_trivially_copyable_v, true_type

using namespace xstd;

BOOST_AUTO_TEST_SUITE(TypeTraits)

template<class T>
using is_complex = is_specialization_of<T, std::complex>;

template<class T>
inline constexpr auto is_complex_v = is_complex<T>::value;

BOOST_AUTO_TEST_CASE(IsSpecializationOf)
{
        XSTD_CONSTEXPR_CHECK((is_complex_v<std::complex<int>>));
        XSTD_CONSTEXPR_CHECK((not is_complex_v<int>));
}

template<int N>
using int_ = std::integral_constant<int, N>;

enum class color : unsigned { red = 1 };

template<color N>
using color_ = std::integral_constant<color, N>;

BOOST_AUTO_TEST_CASE(IsIntegralConstant)
{
        XSTD_CONSTEXPR_CHECK((is_integral_constant_v<std::true_type, bool>));
        XSTD_CONSTEXPR_CHECK((is_integral_constant_v<std::false_type, bool>));
        XSTD_CONSTEXPR_CHECK((not is_integral_constant_v<bool, bool>));

        XSTD_CONSTEXPR_CHECK((is_integral_constant_v<int_<0>, int>));
        XSTD_CONSTEXPR_CHECK((not is_integral_constant_v<int, int>));

        // std::integral_constant's first parameter is any type usable as a
        // non-type template parameter, not just an integral one, and the
        // enum case is what xstd::to_underlying is built on. Pinned here
        // because narrowing this trait to std::integral would silently
        // break that without failing any of the checks above.
        XSTD_CONSTEXPR_CHECK((is_integral_constant_v<color_<color::red>, color>));
        XSTD_CONSTEXPR_CHECK((not is_integral_constant_v<color, color>));

        // the wrapped type has to match: an integral_constant over one type
        // is not one over another
        XSTD_CONSTEXPR_CHECK((not is_integral_constant_v<int_<0>, unsigned>));
        XSTD_CONSTEXPR_CHECK((not is_integral_constant_v<color_<color::red>, unsigned>));
}

struct tag1;
struct tag2;

BOOST_AUTO_TEST_CASE(EmptyType)
{
        using empty1 = empty_type<tag1>;
        using empty2 = empty_type<tag2>;

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

// the tag can be declared in place, in the template argument list itself,
// which is how a class with conditional members names each of them without
// a separate declaration per tag. Both stand in for the same Type here:
// that is exactly the case an unwritable tag would let collide.
using member1 = conditional_data_member_t<false, tag1, struct member1_tag>;
using member2 = conditional_data_member_t<false, tag1, struct member2_tag>;

BOOST_AUTO_TEST_CASE(ConditionalDataMember)
{
        XSTD_CONSTEXPR_CHECK((std::is_same_v<conditional_data_member_t<true, tag1, tag2>, tag1>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<conditional_data_member_t<false, tag1, tag2>, empty_type<tag2>>));
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<conditional_data_member_t<false, tag1, tag2>>));

        // the tag names the member, not the type it stands in for, so two
        // absent members over the same Type still get distinct empty types
        // and can go on overlapping in the layout
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<member1>));
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<member2>));
        XSTD_CONSTEXPR_CHECK((not std::is_same_v<member1, member2>));

        // the tag is inert when the member is present: same Type, same tags
        // as above, and the two agree once the condition holds
        XSTD_CONSTEXPR_CHECK((std::is_same_v<
                              conditional_data_member_t<true, tag1, struct member1_tag>,
                              conditional_data_member_t<true, tag1, struct member2_tag>>));

        // an absent member never needs its tag defined
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<conditional_data_member_t<false, tag1, struct undefined_tag>>));
}

BOOST_AUTO_TEST_SUITE_END()
