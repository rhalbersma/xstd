//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts.hpp>        // enumeration, specialization_of
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK
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
constexpr auto as_complex(T z) noexcept -> T
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

        // the concept agrees with the trait it is spelled over
        XSTD_CONSTEXPR_CHECK((specialization_of<std::complex<int>, std::complex> == is_specialization_of_v<std::complex<int>, std::complex>));
        XSTD_CONSTEXPR_CHECK((specialization_of<int, std::complex> == is_specialization_of_v<int, std::complex>));

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK(has_as_complex<std::complex<int>>);
        XSTD_CONSTEXPR_CHECK(!has_as_complex<int>);
        XSTD_CONSTEXPR_CHECK(as_complex(std::complex<int>(1, 2)) == std::complex<int>(1, 2));
}

BOOST_AUTO_TEST_SUITE_END()
