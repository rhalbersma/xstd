//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/specialization_of.hpp> // specialization_of
#include <xstd/test/constexpr_check.hpp>       // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                             // complex
#include <tuple>                               // tuple

BOOST_AUTO_TEST_SUITE(Concepts)

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

        // used as a type-constraint, it constrains rather than hard-errors
        XSTD_CONSTEXPR_CHECK(has_as_complex<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_as_complex<int>);

        // and it runs; double rather than int, the MSVC STL deprecating the others
        XSTD_CONSTEXPR_CHECK_EQUAL((as_complex(std::complex<double>{1.0, 2.0})), (std::complex<double>{1.0, 2.0}));
}

BOOST_AUTO_TEST_SUITE_END()
