//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/charconv/to_chars.hpp> // to_chars, to_chars_max_size, detail::has_std_to_chars
#include <xstd/cstdint.hpp>           // int128, uint128
#include <boost/test/unit_test.hpp>   // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                      // array
#include <charconv>                   // to_chars
#include <cstdint>                    // exact-width integer types
#include <limits>                     // numeric_limits
#include <string>                     // string
#include <string_view>                // string_view
#include <system_error>               // errc
#include <tuple>                      // tuple

BOOST_AUTO_TEST_SUITE(CharConvToChars)

// A named concept rather than a bare requires-expression at the use: selecting
// a deleted function makes the expression ill-formed, and an ill-formed
// operand that is also non-dependent is a hard error rather than an
// unsatisfied constraint. Keeping the type a template parameter is what turns
// the deleted overload into the false answer the check is looking for.
template<class T>
concept has_xstd_to_chars = requires (char* p, T value) { xstd::to_chars(p, p, value, 10); };

using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t, xstd::int128>;
using unsigned_types = std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, xstd::uint128>;

// Named rather than spelled inline at the use: the preprocessor splits a
// macro argument on the tuple's commas.
using standard_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

template<class T>
[[nodiscard]] auto rendered(T value, int base)
        -> std::string
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        BOOST_CHECK(result.ec == std::errc{});
        return std::string(buffer.data(), result.ptr);
}

// The standard's, for the types it covers, so the two can be compared. Sized
// generously rather than by to_chars_max_size, to keep the comparison honest.
template<class T>
[[nodiscard]] auto rendered_by_std(T value, int base)
        -> std::string
{
        auto buffer = std::array<char, 160>{};
        auto const result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        BOOST_CHECK(result.ec == std::errc{});
        return std::string(buffer.data(), result.ptr);
}

// Which path a type takes is a property of the standard library and the
// dialect, not of xstd: libstdc++ generates its to_chars overload set per
// built-in width and gates the 128-bit pair on __GLIBCXX_TYPE_INT_N_0, which
// the compiler only predefines outside __STRICT_ANSI__. So this is not asserted
// for the 128-bit types - only that both paths exist and agree.
BOOST_AUTO_TEST_CASE(DelegatesWhereTheStandardLibraryCovers)
{
        static_assert(xstd::detail::has_std_to_chars<int>);
        static_assert(xstd::detail::has_std_to_chars<long long>);
        static_assert(xstd::detail::has_std_to_chars<unsigned>);

        // bool is integral-like, but the standard deletes its to_chars and so
        // does xstd, rather than letting it fall through to the digits path.
        static_assert(not xstd::detail::has_std_to_chars<bool>);
        static_assert(not has_xstd_to_chars<bool>);
        static_assert(has_xstd_to_chars<int>);

        // A type with no overload anywhere takes the digits path rather than
        // being a hard error - the ambiguous case has to survive too, which is
        // what libstdc++ produces for __int128 in the strict dialect.
        struct not_an_integer
        {};
        static_assert(not xstd::detail::has_std_to_chars<not_an_integer>);
}

// The load-bearing property: wherever the standard library covers the type,
// the digits path must be byte-identical to it, at every base. A divergence
// would show up only on the platforms that take the other branch, which are
// exactly the ones hardest to notice it on.
BOOST_AUTO_TEST_CASE_TEMPLATE(DigitsPathMatchesTheStandard, T, standard_width_types)
{
        for (auto base = 2; base <= 36; ++base) {
                for (auto const value : {T{0}, T{1}, T{-1}, T{7}, T{-7},
                                         std::numeric_limits<T>::min(), std::numeric_limits<T>::max()}) {
                        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
                        auto const result = xstd::detail::to_chars_fallback(buffer.data(), buffer.data() + buffer.size(), value, base);
                        BOOST_CHECK(result.ec == std::errc{});
                        BOOST_CHECK_EQUAL(std::string(buffer.data(), result.ptr),
                                          rendered_by_std(value, base));
                }
        }
}

// to_chars_max_size has to hold the worst case, which is min() in base 2. The
// trap is that digits counts value bits only, so a signed type needs two more
// characters than that, not one.
BOOST_AUTO_TEST_CASE_TEMPLATE(MaxSizeHoldsTheWorstCase, T, exact_width_types)
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const min = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<T>::min(), 2);
        BOOST_CHECK(min.ec == std::errc{});
        auto const max = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<T>::max(), 2);
        BOOST_CHECK(max.ec == std::errc{});
}

BOOST_AUTO_TEST_CASE_TEMPLATE(MaxSizeHoldsTheWorstCaseUnsigned, T, unsigned_types)
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const max = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<T>::max(), 2);
        BOOST_CHECK(max.ec == std::errc{});
}

BOOST_AUTO_TEST_CASE(Int128Boundaries)
{
        BOOST_CHECK_EQUAL(rendered(std::numeric_limits<xstd::int128>::min(), 10), "-170141183460469231731687303715884105728");
        BOOST_CHECK_EQUAL(rendered(std::numeric_limits<xstd::int128>::max(), 10), "170141183460469231731687303715884105727");
        BOOST_CHECK_EQUAL(rendered(std::numeric_limits<xstd::uint128>::max(), 10), "340282366920938463463374607431768211455");
        BOOST_CHECK_EQUAL(rendered(std::numeric_limits<xstd::int128>::max(), 16), "7fffffffffffffffffffffffffffffff");
        BOOST_CHECK_EQUAL(rendered(std::numeric_limits<xstd::uint128>::max(), 2), std::string(128, '1'));
        BOOST_CHECK_EQUAL(rendered(xstd::int128{0}, 10), "0");
        BOOST_CHECK_EQUAL(rendered(xstd::int128{-1}, 10), "-1");
        BOOST_CHECK_EQUAL(rendered(xstd::int128{35}, 36), "z");
}

// A constant expression, not merely a constexpr function called at runtime:
// that is the property std::formatter<div_t<S>>::format needs in order to opt
// into P3391, and the one a std::to_string-based version could never have.
template<class T>
[[nodiscard]] consteval auto rendered_at_compile_time(T value, int base, std::string_view expected)
        -> bool
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        return result.ec == std::errc{} and std::string_view{buffer.data(), result.ptr} == expected;
}

BOOST_AUTO_TEST_CASE(UsableInAConstantExpression)
{
        static_assert(rendered_at_compile_time(255, 16, "ff"));
        static_assert(rendered_at_compile_time(-42, 10, "-42"));
        static_assert(rendered_at_compile_time(std::numeric_limits<xstd::int128>::min(), 10,
                                               "-170141183460469231731687303715884105728"));
        static_assert(rendered_at_compile_time(std::numeric_limits<xstd::uint128>::max(), 16,
                                               "ffffffffffffffffffffffffffffffff"));
}

// A buffer too small reports value_too_large and leaves ptr at last, rather
// than writing a truncated answer.
BOOST_AUTO_TEST_CASE(ShortBuffer)
{
        auto buffer = std::array<char, 4>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), 123456, 10);
        BOOST_CHECK(result.ec == std::errc::value_too_large);
        BOOST_CHECK(result.ptr == buffer.data() + buffer.size());

        auto const wide = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<xstd::int128>::min(), 10);
        BOOST_CHECK(wide.ec == std::errc::value_too_large);
}

BOOST_AUTO_TEST_SUITE_END()
