//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer_class.hpp>    // integer_class
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/test/exact_width_types.hpp>         // exact_width_integer_types
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <boost/test/unit_test.hpp>                // Boost.Test

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(IntegerClass)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheExactWidthTypesThroughCv, T, xstd::test::exact_width_integer_types)
{
        static_assert(xstd::integer_class<T>);
        static_assert(xstd::integer_class<T const>);
        static_assert(xstd::integer_class<T volatile>);
        static_assert(xstd::integer_class<T const volatile>);
        BOOST_CHECK(true);
}

// The requirements hold of the types they were modelled on, which the old
// (not std::integral) clause made unaskable rather than false. `integer` now
// rests on this: it dropped its std::integral disjunct as redundant.
BOOST_AUTO_TEST_CASE(StandardIntegral)
{
        static_assert(xstd::integer_class<char>);
        static_assert(xstd::integer_class<signed char>);
        static_assert(xstd::integer_class<unsigned char>);
        static_assert(xstd::integer_class<char8_t>);
        static_assert(xstd::integer_class<char16_t>);
        static_assert(xstd::integer_class<char32_t>);
        static_assert(xstd::integer_class<wchar_t>);
        static_assert(xstd::integer_class<short>);
        static_assert(xstd::integer_class<unsigned short>);
        static_assert(xstd::integer_class<int>);
        static_assert(xstd::integer_class<unsigned int>);
        static_assert(xstd::integer_class<long>);
        static_assert(xstd::integer_class<unsigned long>);
        static_assert(xstd::integer_class<long long>);
        static_assert(xstd::integer_class<unsigned long long>);

        // C++17 removed bool's ++ and --, which /7.1 and /7.2 ask for.
        static_assert(not xstd::integer_class<bool>);
        BOOST_CHECK(true);
}

// /3's other half, which xstd does check: the range is -2^(N-1) to 2^(N-1)-1
// signed and 0 to 2^N-1 unsigned. Read through the pair, so that no expression
// here has to survive integral promotion or overflow at the widest type - and
// over the signed list alone, since naming T's pair reaches every type once.
BOOST_AUTO_TEST_CASE_TEMPLATE(RepresentableRange, T, xstd::test::constexpr_exact_width_signed_integer_types)
{
        using S = xstd::make_signed_t<T>;
        using U = xstd::make_unsigned_t<T>;

        // min is -(max + 1), the asymmetry that -2^(N-1) to 2^(N-1)-1 states.
        static_assert(xstd::numeric_limits<S>::min() + xstd::numeric_limits<S>::max() == -1);
        static_assert(xstd::numeric_limits<U>::min() == 0);

        // The unsigned half spends on magnitude the bit the signed half spends on sign.
        static_assert(xstd::numeric_limits<U>::digits == xstd::numeric_limits<S>::digits + 1);
        static_assert((static_cast<U>(xstd::numeric_limits<S>::max()) * 2) + 1 == xstd::numeric_limits<U>::max());
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
