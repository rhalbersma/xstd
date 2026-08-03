//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>        // is_specialization_of, is_integral_constant, empty_type, conditional_data_member_t, make_signed_like_t, make_unsigned_like_t, is_arithmetic_like_v, is_integral_like, is_integral_like_v, is_signed_like_v, is_unsigned_like_v
#include <xstd/test/constexpr.hpp>     // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                     // strong_ordering
#include <complex>                     // complex
#include <cstdint>                     // int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t
#include <limits>                      // numeric_limits
#include <type_traits>                 // false_type, integral_constant, is_arithmetic_v, is_constructible_v, is_convertible_v, is_empty_v, is_integral_v, is_nothrow_constructible_v, is_nothrow_default_constructible_v, is_same_v, is_signed_v, is_trivially_constructible_v, is_trivially_copyable_v, is_unsigned_v, make_unsigned_t, true_type

// An incomplete class type is answered, not hard-errored: the traits being
// widened cope with one, so these have to as well. It is the sizeof term in
// xstd::exposition_only::integral_class_type that keeps them total - every
// question after it, std::regular's std::destructible first, needs a complete
// type to be well-formed rather than merely false.
BOOST_AUTO_TEST_CASE(OpenedNumericTraitsAnswerForIncompleteTypes)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<struct never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_arithmetic_like_v<struct never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_signed_like_v<struct never_defined>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_unsigned_like_v<struct never_defined>);
}

// A class type that says it is a number but not an integer - the shape a
// future floating-point-class opening would cover. Only what its
// std::numeric_limits reports matters, so it needs no members and no
// operators: the specialization is the whole fixture. Both live out here
// because an explicit specialization of a standard-library template has to be
// at global scope, and BOOST_AUTO_TEST_SUITE opens a namespace.
struct not_an_integer_class_type {};

template<>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2 for a program-defined type
struct std::numeric_limits<not_an_integer_class_type> : std::numeric_limits<double> {};

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

// The four opened numeric traits. What makes them widenings rather than
// replacements is that they agree with the standard's on every type the
// standard's can answer for, so these are checked against std::is_arithmetic_v
// / std::is_signed_v / std::is_unsigned_v / std::is_integral_v directly rather
// than against hand-written expectations - a table of expected answers could
// drift, an equality against the trait being widened cannot.
template<class T>
auto check_agrees_with_std()
        -> void
{
        XSTD_CONSTEXPR_CHECK(xstd::is_arithmetic_like_v<T> == std::is_arithmetic_v<T>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<T> == std::is_signed_v<T>);
        XSTD_CONSTEXPR_CHECK(xstd::is_unsigned_like_v<T> == std::is_unsigned_v<T>);
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<T> == std::is_integral_v<T>);
}

BOOST_AUTO_TEST_CASE(OpenedNumericTraitsAgreeWithStd)
{
        check_agrees_with_std<bool>();
        check_agrees_with_std<char>();
        check_agrees_with_std<signed char>();
        check_agrees_with_std<unsigned char>();
        check_agrees_with_std<short>();
        check_agrees_with_std<int>();
        check_agrees_with_std<unsigned>();
        check_agrees_with_std<long long>();
        check_agrees_with_std<float>();
        check_agrees_with_std<double>();

        // and on the types neither can call arithmetic
        check_agrees_with_std<int*>();
        check_agrees_with_std<color>();
        check_agrees_with_std<std::complex<double>>();
        check_agrees_with_std<void>();
        check_agrees_with_std<int&>();
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        check_agrees_with_std<int[3]>();
}

// The fourth opened trait, and the one whose standard counterpart answers
// from a closed list rather than from a property: std::is_integral_v is
// extended with [iterator.concept.winc]'s integer-class types, opened
// structurally in <xstd/exposition_only.hpp> and pinned in
// src/exposition_only.cpp. The four cases below check the trait spelling
// itself. Agreement with std::is_integral_v wherever it can answer is already
// covered by the sweep above, which this trait joins.
BOOST_AUTO_TEST_CASE(IsIntegralLike)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like_v<char32_t>);

        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<color>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<std::complex<double>>);
}

// Total where the requirements it reaches are ill-formed rather than merely
// unsatisfied: they sit inside a concept, whose conjunction short-circuits, so
// each of these is an answer and not a compile error.
BOOST_AUTO_TEST_CASE(IsIntegralLikeIsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<int()>);
}

// The C++23 category traits are cv-transparent, and so is the widening.
BOOST_AUTO_TEST_CASE(IsIntegralLikeIncludesCvQualifiedTypes)
{
        XSTD_CONSTEXPR_CHECK(std::is_integral_v<int const> and xstd::is_integral_like_v<int const>);
        XSTD_CONSTEXPR_CHECK(std::is_integral_v<int volatile> and xstd::is_integral_like_v<int volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::is_arithmetic_like_v<double const>);
}

// The bool_constant form, which is what std::conjunction and tag dispatch want
// and a concept cannot be.
BOOST_AUTO_TEST_CASE(IsIntegralLikeBoolConstant)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_integral_like<int>::value);
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::is_integral_like<int>, std::true_type>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::is_integral_like<double>, std::false_type>));
}

// The half xstd has *not* opened. is_arithmetic_like_v is spelled the way the
// standard spells is_arithmetic_v - an integral type, or a floating-point one
// - with only the integral half replaced by an open one, so a class type that
// behaves like a floating-point number is not arithmetic-like today. It is the
// obvious next thing to open, and the shape of the trait is what leaves room
// for it: a second exposition-only concept, a second disjunct, nothing else
// moved.
BOOST_AUTO_TEST_CASE(ArithmeticLikeOpensTheIntegralHalfOnly)
{
        XSTD_CONSTEXPR_CHECK(std::numeric_limits<not_an_integer_class_type>::is_specialized);
        XSTD_CONSTEXPR_CHECK(not std::numeric_limits<not_an_integer_class_type>::is_integer);

        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<not_an_integer_class_type>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_arithmetic_like_v<not_an_integer_class_type>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_signed_like_v<not_an_integer_class_type>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_unsigned_like_v<not_an_integer_class_type>);

        // the built-in floating-point types are the standard's half, reported
        // exactly as the standard reports them
        XSTD_CONSTEXPR_CHECK(xstd::is_arithmetic_like_v<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_integral_like_v<double>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_like_v<double>);
}

// A named concept rather than a bare requires-expression in the test body: a
// requires-expression whose operand is invalid *and* non-dependent is a hard
// error on GCC, so the type has to stay a template parameter.
template<class T>
concept has_make_unsigned_like = requires { typename xstd::make_unsigned_like_t<T>; };

template<class T>
concept has_make_signed_like = requires { typename xstd::make_signed_like_t<T>; };

BOOST_AUTO_TEST_CASE(MakeSignedAndUnsignedLike)
{
        // The signed half is the exact mirror of the unsigned half.
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<std::uint8_t>, std::int8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<unsigned>, int>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<unsigned const>, int const>));
        XSTD_CONSTEXPR_CHECK(not has_make_signed_like<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_signed_like<double>);

        // agrees with std::make_unsigned wherever std::make_unsigned answers,
        // for the unsigned types as much as the signed ones
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int8_t>, std::uint8_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int16_t>, std::uint16_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int32_t>, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<std::int64_t>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<int>, std::make_unsigned_t<int>>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<unsigned>, unsigned>));

        // and answers "no" where std::make_unsigned is a hard error instead:
        // that is the whole point of an empty primary template. Each of these
        // would stop the compile rather than evaluate to false if the trait
        // inherited from std::make_unsigned unconditionally.
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<double>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<int*>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<color>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned_like<void>);

        // cv-qualification is preserved, just as by std::make_unsigned.
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<int const>, unsigned const>));

        // __int128 is the one built-in type whose std::is_integral answer
        // depends on the dialect, so xstd names its counterpart outright
        // rather than deriving it
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_unsigned_like_t<__int128>, unsigned __int128>));
        XSTD_CONSTEXPR_CHECK((std::is_same_v<xstd::make_signed_like_t<unsigned __int128>, __int128>));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
}

BOOST_AUTO_TEST_SUITE_END()
