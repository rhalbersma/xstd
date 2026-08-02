//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>           // enumeration, specialization_of, integral_like, signed_integral_like, unsigned_integral_like
#include <xstd/type_traits.hpp>        // empty_type, is_integral_like_v, is_specialization_of_v
#include <xstd/test/constexpr.hpp>     // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/integer_class.hpp> // signed_integer_class, unsigned_integer_class
#include <boost/test/unit_test.hpp>    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK
#include <complex>                     // complex
#include <concepts>                    // integral, signed_integral, unsigned_integral
#include <cstdint>                     // int8_t, int16_t, int32_t, int64_t
#include <tuple>                       // tuple
#include <type_traits>                 // make_unsigned_t

BOOST_AUTO_TEST_SUITE(Concepts)

enum unscoped { u0 };
enum class scoped : unsigned { s0 };
struct not_an_enum
{};

BOOST_AUTO_TEST_CASE(Enumeration)
{
        XSTD_CONSTEXPR_CHECK(xstd::enumeration<unscoped>);
        XSTD_CONSTEXPR_CHECK(xstd::enumeration<scoped>);

        XSTD_CONSTEXPR_CHECK(not xstd::enumeration<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::enumeration<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::enumeration<not_an_enum>);
        XSTD_CONSTEXPR_CHECK(not xstd::enumeration<scoped&>);
        XSTD_CONSTEXPR_CHECK(not xstd::enumeration<scoped*>);
        XSTD_CONSTEXPR_CHECK(not xstd::enumeration<void>);
}

// the partial application a type-constraint needs: the primary template alone
template<xstd::specialization_of<std::complex> T>
[[nodiscard]] constexpr auto as_complex(T z) noexcept
        -> T
{
        return z;
}

template<class T>
concept has_as_complex = requires (T t) { as_complex(t); };

BOOST_AUTO_TEST_CASE(SpecializationOf)
{
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::tuple<int, char>, std::tuple>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::tuple<>, std::tuple>));

        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<int, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<std::tuple<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<std::complex<int>&, std::complex>));

        // nothing in the concept is specific to the standard library. The
        // library's own class template is a program-defined primary like any
        // other, with its tag declared in place
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<xstd::empty_type<struct user_tag>, xstd::empty_type>));
        XSTD_CONSTEXPR_CHECK((not xstd::specialization_of<int, xstd::empty_type>));

        // the concept agrees with the trait it is spelled over
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<std::complex<int>, std::complex> == xstd::is_specialization_of_v<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((xstd::specialization_of<int, std::complex> == xstd::is_specialization_of_v<int, std::complex>));

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK(has_as_complex<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_as_complex<int>);

        // and the constrained template actually runs for a matching argument,
        // which the two checks above cannot show: a requires-expression's
        // operand is unevaluated, so a body that is ill-formed on
        // instantiation would still leave has_as_complex true. double rather
        // than int as the element type because the MSVC STL deprecates
        // std::complex's constructor for anything but the floating-point ones
        // (STL4037) and these tests build with warnings as errors; merely
        // naming std::complex<int> stays fine, which is all the checks above
        // do.
        XSTD_CONSTEXPR_CHECK_EQUAL((as_complex(std::complex<double>{1.0, 2.0})), (std::complex<double>{1.0, 2.0}));
}

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

// The property that makes this a widening of <concepts> rather than a
// replacement for it: whatever std::integral accepts, integral_like accepts,
// and with the same signedness. Checked per width rather than once, since the
// signed and unsigned answers have to line up at each.
BOOST_AUTO_TEST_CASE_TEMPLATE(IntegralLikeIsASupersetOfIntegral, T, exact_width_types)
{
        using U = std::make_unsigned_t<T>;

        static_assert(std::signed_integral<T> and xstd::signed_integral_like<T>);
        static_assert(std::unsigned_integral<U> and xstd::unsigned_integral_like<U>);

        static_assert(xstd::integral_like<T> and xstd::integral_like<U>);
        static_assert(not xstd::unsigned_integral_like<T>);
        static_assert(not xstd::signed_integral_like<U>);

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

BOOST_AUTO_TEST_CASE(IntegralLike)
{
        // the built-in character and boolean types, which std::integral also
        // accepts. bool comes out unsigned, exactly as std::unsigned_integral
        // <bool> already holds - these concepts widen the standard ones
        // rather than tidy them up
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<bool> and xstd::unsigned_integral_like<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::signed_integral_like<bool>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<char> and xstd::integral_like<char32_t>);

        // nothing whose std::numeric_limits says it is not an integer
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<not_an_enum>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<scoped>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int*>);

        // the spellings that are answered rather than hard-errored only
        // because the requirements they would trip over sit inside a concept,
        // where a conjunction short-circuits. src/exposition_only.cpp pins
        // the ordering that makes that work; these four pin that the concept
        // a caller actually writes inherits it.
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int()>);

        // cv-qualified types are outside the trait's domain, which is the one
        // place these concepts narrow rather than widen the standard's:
        // std::integral<int const> holds. It is why xstd::make_unsigned_like
        // has nothing to say about cv-qualified types either
        XSTD_CONSTEXPR_CHECK(not xstd::integral_like<int const>);
}

// The concept agrees with the trait it is spelled over, on the types where
// that spelling is what keeps the question answerable at all: the trait's own
// answers, and the reasons they are answers rather than compile errors, are
// pinned in src/type_traits.cpp, where it is defined.
BOOST_AUTO_TEST_CASE(IntegralLikeAgreesWithItsTrait)
{
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int> == xstd::is_integral_like_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<double> == xstd::is_integral_like_v<double>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<void> == xstd::is_integral_like_v<void>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<int[3]> == xstd::is_integral_like_v<int[3]>);
        XSTD_CONSTEXPR_CHECK(xstd::integral_like<xstd::test::signed_integer_class> == xstd::is_integral_like_v<xstd::test::signed_integer_class>);
}

// Overloading on integral_like and signed_integral_like has to partial-order
// the way std::integral and std::signed_integral do. It only does so because
// each of the narrower concepts is spelled "integral_like<T> and ...", so its
// normal form contains integral_like's; a formulation that repeated the
// requirements, or that gave either of the narrower concepts a flat trait of
// its own to be spelled over, would make this call ambiguous rather than
// picking the second overload.
template<xstd::integral_like T>
[[nodiscard]] constexpr auto which(T) noexcept -> int
{
        return 1;
}

template<xstd::signed_integral_like T>
[[nodiscard]] constexpr auto which(T) noexcept -> int
{
        return 2;
}

BOOST_AUTO_TEST_CASE(NarrowerConceptsSubsume)
{
        XSTD_CONSTEXPR_CHECK(which(1) == 2);
        XSTD_CONSTEXPR_CHECK(which(1U) == 1);
        XSTD_CONSTEXPR_CHECK(which(xstd::test::signed_integer_class{1}) == 2);
        XSTD_CONSTEXPR_CHECK(which(xstd::test::unsigned_integer_class{1}) == 1);
}

// The shape std::signed_integral can never describe, on any compiler or
// dialect: a class type. It qualifies here by supplying the operators, the
// std::numeric_limits specialization and the xstd::make_unsigned_like
// specialization - by behaving like an integer rather than by being on a
// list.
BOOST_AUTO_TEST_CASE(IntegerClassTypes)
{
        using S = xstd::test::signed_integer_class;
        using U = xstd::test::unsigned_integer_class;

        XSTD_CONSTEXPR_CHECK(not std::integral<S> and not std::integral<U>);

        XSTD_CONSTEXPR_CHECK(xstd::integral_like<S> and xstd::signed_integral_like<S>);
        XSTD_CONSTEXPR_CHECK(not xstd::unsigned_integral_like<S>);

        XSTD_CONSTEXPR_CHECK(xstd::integral_like<U> and xstd::unsigned_integral_like<U>);
        // U is a perfectly good unsigned integer-like type, but no type names
        // it as *its* counterpart, and it has none of its own
        XSTD_CONSTEXPR_CHECK(not xstd::signed_integral_like<U>);
}

BOOST_AUTO_TEST_SUITE_END()
