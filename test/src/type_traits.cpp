//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/type_traits.hpp>         // is_specialization_of, is_integral_constant, tagged_empty, optional_type
#include <xstd/test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                      // complex
#include <type_traits>                  // integral_constant, is_constructible_v, is_convertible_v, is_empty_v, is_same_v, is_trivially_constructible_v, is_trivially_copyable_v

using namespace xstd;

BOOST_AUTO_TEST_SUITE(TypeTraits)

template<class T>
using is_complex = is_specialization_of<T, std::complex>;

template<class T>
inline constexpr auto is_complex_v = is_complex<T>::value;

BOOST_AUTO_TEST_CASE(IsSpecializationOf)
{
        XSTD_CONSTEXPR_CHECK(( is_complex_v<std::complex<int>>));
        XSTD_CONSTEXPR_CHECK((!is_complex_v<int>));
}

template<int N>
using int_ = std::integral_constant<int, N>;

BOOST_AUTO_TEST_CASE(IsIntegralConstant)
{
        XSTD_CONSTEXPR_CHECK(( is_integral_constant_v<std:: true_type, bool>));
        XSTD_CONSTEXPR_CHECK(( is_integral_constant_v<std::false_type, bool>));
        XSTD_CONSTEXPR_CHECK((!is_integral_constant_v<bool, bool>));

        XSTD_CONSTEXPR_CHECK(( is_integral_constant_v<int_<0>, int>));
        XSTD_CONSTEXPR_CHECK((!is_integral_constant_v<int,     int>));
}

struct tag1;
struct tag2;

BOOST_AUTO_TEST_CASE(TaggedEmpty)
{
        using empty1 = tagged_empty<tag1>;
        using empty2 = tagged_empty<tag2>;

        static_assert( std::is_empty_v<empty1>);
        static_assert(!std::is_same_v<empty1, empty2>);

        // constructible from anything, but only explicitly
        static_assert( std::is_constructible_v<empty1, int, double>);
        static_assert(!std::is_convertible_v<int, empty1>);

        // the catch-all constructor never hijacks copy/move construction,
        // not even from a non-const lvalue
        static_assert(std::is_trivially_copyable_v<empty1>);
        static_assert(std::is_trivially_constructible_v<empty1, empty1&>);
        static_assert(std::is_trivially_constructible_v<empty1, empty1 const&>);
        static_assert(std::is_trivially_constructible_v<empty1, empty1&&>);

        // stateless: all instances compare equal, regardless of construction
        XSTD_CONSTEXPR_CHECK(empty1(1, 2.0) == empty1());
        XSTD_CONSTEXPR_CHECK(empty1() == empty1(42));
}

BOOST_AUTO_TEST_CASE(OptionalType)
{
        XSTD_CONSTEXPR_CHECK((std::is_same_v<optional_type<true,  tag1>, tag1>));
        static_assert(std::is_same_v<optional_type<false, tag1>, tagged_empty<tag1>>);
        static_assert(std::is_empty_v<optional_type<false, tag1>>);
}

BOOST_AUTO_TEST_SUITE_END()
