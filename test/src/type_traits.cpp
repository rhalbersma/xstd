//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>        // is_specialization_of, is_integral_constant, empty_type, conditional_data_member_t, unsigned_counterpart_t
#include <xstd/test/constexpr.hpp>     // XSTD_CONSTEXPR_CHECK
#include <xstd/test/integer_class.hpp> // signed_integer_class, unsigned_integer_class
#include <boost/test/unit_test.hpp>    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                     // strong_ordering
#include <complex>                     // complex
#include <cstdint>                     // int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t
#include <type_traits>                 // false_type, integral_constant, is_constructible_v, is_convertible_v, is_empty_v, is_nothrow_constructible_v, is_nothrow_default_constructible_v, is_same_v, is_trivially_constructible_v, is_trivially_copyable_v, make_unsigned_t, true_type

BOOST_AUTO_TEST_SUITE(TypeTraits)

template<class T>
using is_complex = xstd::is_specialization_of<T, std::complex>;

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
        XSTD_CONSTEXPR_CHECK((xstd::is_integral_constant_v<std::true_type, bool>));
        XSTD_CONSTEXPR_CHECK((xstd::is_integral_constant_v<std::false_type, bool>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_integral_constant_v<bool, bool>));

        XSTD_CONSTEXPR_CHECK((xstd::is_integral_constant_v<int_<0>, int>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_integral_constant_v<int, int>));

        // std::integral_constant's first parameter is any type usable as a
        // non-type template parameter, not just an integral one, and the
        // enum case is what xstd::to_underlying is built on. Pinned here
        // because narrowing this trait to std::integral would silently
        // break that without failing any of the checks above.
        XSTD_CONSTEXPR_CHECK((xstd::is_integral_constant_v<color_<color::red>, color>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_integral_constant_v<color, color>));

        // the wrapped type has to match: an integral_constant over one type
        // is not one over another
        XSTD_CONSTEXPR_CHECK((not xstd::is_integral_constant_v<int_<0>, unsigned>));
        XSTD_CONSTEXPR_CHECK((not xstd::is_integral_constant_v<color_<color::red>, unsigned>));
}

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

// the tag can be declared in place, in the template argument list itself,
// which is how a class with conditional members names each of them without
// a separate declaration per tag. Both stand in for the same Type here:
// that is exactly the case an unwritable tag would let collide.
using member1 = xstd::conditional_data_member_t<false, tag1, struct member1_tag>;
using member2 = xstd::conditional_data_member_t<false, tag1, struct member2_tag>;

BOOST_AUTO_TEST_CASE(ConditionalDataMember)
{
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::conditional_data_member_t<true, tag1, tag2>, tag1>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::conditional_data_member_t<false, tag1, tag2>, xstd::empty_type<tag2>>));
        XSTD_CONSTEXPR_CHECK((std::is_empty_v<xstd::conditional_data_member_t<false, tag1, tag2>>));

        // the tag names the member, not the type it stands in for, so two
        // absent members over the same Type still get distinct empty types
        // and can go on overlapping in the layout
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

// A named concept rather than a bare requires-expression in the test body: a
// requires-expression whose operand is invalid *and* non-dependent is a hard
// error on GCC, so the type has to stay a template parameter.
template<class T>
concept has_unsigned_counterpart = requires { typename xstd::unsigned_counterpart_t<T>; };

BOOST_AUTO_TEST_CASE(UnsignedCounterpart)
{
        // agrees with std::make_unsigned wherever std::make_unsigned answers
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<std::int8_t>, std::uint8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<std::int16_t>, std::uint16_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<std::int32_t>, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<std::int64_t>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<int>, std::make_unsigned_t<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<unsigned>, unsigned>));

        // and answers "no" where std::make_unsigned is a hard error instead:
        // that is the whole point of an empty primary template. Each of these
        // would stop the compile rather than evaluate to false if the trait
        // inherited from std::make_unsigned unconditionally.
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<bool>);
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<double>);
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<int*>);
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<color>);
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<void>);

        // cv-qualified types are outside the trait's domain, unlike
        // std::make_unsigned's: they are not std::regular, so no cv-qualified
        // type is xstd::integer_like and none would ever be asked
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<int const>);

        // a class type answers through a user-supplied specialization, which
        // is what the empty primary template leaves room for
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<xstd::test::signed_integer_class>, xstd::test::unsigned_integer_class>));
        XSTD_CONSTEXPR_CHECK(not has_unsigned_counterpart<xstd::test::unsigned_integer_class>);

        // __int128 is the one built-in type whose std::is_integral answer
        // depends on the dialect, so xstd names its counterpart outright
        // rather than deriving it
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::unsigned_counterpart_t<__int128>, unsigned __int128>));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
}

BOOST_AUTO_TEST_SUITE_END()
