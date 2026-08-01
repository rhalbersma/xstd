//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>        // enumeration, specialization_of
#include <xstd/type_traits.hpp>     // empty_type, is_specialization_of_v
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                  // complex
#include <tuple>                    // tuple

using namespace xstd;

BOOST_AUTO_TEST_SUITE(Concepts)

enum unscoped { u0 };
enum class scoped : unsigned { s0 };
struct not_an_enum
{};

BOOST_AUTO_TEST_CASE(Enumeration)
{
        XSTD_CONSTEXPR_CHECK(enumeration<unscoped>);
        XSTD_CONSTEXPR_CHECK(enumeration<scoped>);

        XSTD_CONSTEXPR_CHECK(!enumeration<int>);
        XSTD_CONSTEXPR_CHECK(!enumeration<bool>);
        XSTD_CONSTEXPR_CHECK(!enumeration<not_an_enum>);
        XSTD_CONSTEXPR_CHECK(!enumeration<scoped&>);
        XSTD_CONSTEXPR_CHECK(!enumeration<scoped*>);
        XSTD_CONSTEXPR_CHECK(!enumeration<void>);
}

// the partial application a type-constraint needs: the primary template alone
template<specialization_of<std::complex> T>
[[nodiscard]] constexpr auto as_complex(T z) noexcept
        -> T
{
        return z;
}

template<class T>
concept has_as_complex = requires (T t) { as_complex(t); };

BOOST_AUTO_TEST_CASE(SpecializationOf)
{
        XSTD_CONSTEXPR_CHECK((specialization_of<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((specialization_of<std::tuple<int, char>, std::tuple>));
        XSTD_CONSTEXPR_CHECK((specialization_of<std::tuple<>, std::tuple>));

        XSTD_CONSTEXPR_CHECK((!specialization_of<int, std::complex>));
        XSTD_CONSTEXPR_CHECK((!specialization_of<std::tuple<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((!specialization_of<std::complex<int>&, std::complex>));

        // nothing in the concept is specific to the standard library. The
        // library's own class template is a program-defined primary like any
        // other, with its tag declared in place
        XSTD_CONSTEXPR_CHECK((specialization_of<empty_type<struct user_tag>, empty_type>));
        XSTD_CONSTEXPR_CHECK((!specialization_of<int, empty_type>));

        // the concept agrees with the trait it is spelled over
        XSTD_CONSTEXPR_CHECK((specialization_of<std::complex<int>, std::complex> == is_specialization_of_v<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((specialization_of<int, std::complex> == is_specialization_of_v<int, std::complex>));

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK(has_as_complex<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(!has_as_complex<int>);

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

BOOST_AUTO_TEST_SUITE_END()
