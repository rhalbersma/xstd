//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/charconv/to_chars.hpp>      // to_chars, to_chars_max_size
#include <xstd/cstdint.hpp>                // int128, uint128
#include <xstd/test/exact_width_types.hpp> // exact_width_signed_types, exact_width_unsigned_types
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                           // array
#include <charconv>                        // to_chars, to_chars_result
#include <concepts>                        // same_as
#include <cstdint>                         // exact-width integer types
#include <limits>                          // numeric_limits
#include <string>                          // string
#include <string_view>                     // string_view
#include <system_error>                    // errc
#include <tuple>                           // tuple

BOOST_AUTO_TEST_SUITE(CharConvToChars)

// Named concepts rather than bare requires-expressions at the uses below.
// Selecting a deleted function makes an expression ill-formed, an ambiguous
// call likewise, and an ill-formed operand that is also non-dependent is a hard
// error rather than an unsatisfied constraint. Keeping the type a template
// parameter is what turns both into the answer these checks are looking for.
//
// The header spells the second of these inline, as the constraint on its
// delegating overload, and can: there T is that template's own parameter, so
// the expression is dependent where it is written. Here the types are concrete,
// which is the whole point of the checks, so here they have to be named.
template<class T>
concept has_std_to_chars = requires (char* p, T value, int base) {
        { std::to_chars(p, p, value, base) } -> std::same_as<std::to_chars_result>;
};

// Answers a second question too, now that xstd::to_chars is two overloads: an
// ambiguous call is also a call that does not compile, so this being true for a
// type both overloads accept is what pins that the constrained one wins by
// partial ordering rather than tying.
template<class T>
concept has_xstd_to_chars = requires (char* p, T value) { xstd::to_chars(p, p, value, 10); };

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
        static_assert(has_std_to_chars<int>);
        static_assert(has_std_to_chars<long long>);
        static_assert(has_std_to_chars<unsigned>);

        // bool is integral-like, but the standard deletes its to_chars and so
        // does xstd, rather than letting it fall through to the digits path.
        static_assert(not has_std_to_chars<bool>);
        static_assert(not has_xstd_to_chars<bool>);
        static_assert(has_xstd_to_chars<int>);

        // A type with no overload anywhere takes the digits path rather than
        // being a hard error - the ambiguous case has to survive too, which is
        // what libstdc++ produces for __int128 in the strict dialect.
        struct not_an_integer
        {};
        static_assert(not has_std_to_chars<not_an_integer>);
}

// The load-bearing property: wherever the standard library covers a value, the
// digits path must render it byte-identically, at every base. A divergence
// would show up only on the platforms that take the other overload, which are
// exactly the ones hardest to notice it on.
//
// The two overloads are selected by constraint, so the digits path cannot be
// named for a type the standard library covers - the way in is a type it does
// not. xstd::int128 is that type here, and every value of a narrower type is
// one of its values too, so rendering the same value through both is the same
// comparison the widths the standard covers would give. Which overload the
// 128-bit alias takes is a property of the standard library and the dialect
// rather than of xstd, so on an implementation that does cover it this
// compares the standard against itself and still holds.
BOOST_AUTO_TEST_CASE_TEMPLATE(DigitsPathMatchesTheStandard, T, standard_width_types)
{
        for (auto base = 2; base <= 36; ++base) {
                for (auto const value : {T{0}, T{1}, T{-1}, T{7}, T{-7},
                                         std::numeric_limits<T>::min(), std::numeric_limits<T>::max()}) {
                        BOOST_CHECK_EQUAL(rendered(static_cast<xstd::int128>(value), base),
                                          rendered_by_std(value, base));
                }
        }
}

// to_chars_max_size has to hold the worst case, which is min() in base 2. The
// trap is that digits counts value bits only, so a signed type needs two more
// characters than that, not one.
BOOST_AUTO_TEST_CASE_TEMPLATE(MaxSizeHoldsTheWorstCase, T, xstd::test::exact_width_signed_types)
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const min = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<T>::min(), 2);
        BOOST_CHECK(min.ec == std::errc{});
        auto const max = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<T>::max(), 2);
        BOOST_CHECK(max.ec == std::errc{});

        // And holds it exactly: one character less does not fit. Written per
        // type rather than once at a chosen width for the reason the header's
        // overload-ordering comment describes - gcov records a template's
        // instantiations separately, so the short-buffer return is covered
        // only in the instantiations a test actually reaches it in, and a
        // type added to the list above brings its own.
        auto const short_buffer = xstd::to_chars(buffer.data(), buffer.data() + buffer.size() - 1, std::numeric_limits<T>::min(), 2);
        BOOST_CHECK(short_buffer.ec == std::errc::value_too_large);
        BOOST_CHECK(short_buffer.ptr == buffer.data() + buffer.size() - 1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(MaxSizeHoldsTheWorstCaseUnsigned, T, xstd::test::exact_width_unsigned_types)
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const max = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<T>::max(), 2);
        BOOST_CHECK(max.ec == std::errc{});

        auto const short_buffer = xstd::to_chars(buffer.data(), buffer.data() + buffer.size() - 1, std::numeric_limits<T>::max(), 2);
        BOOST_CHECK(short_buffer.ec == std::errc::value_too_large);
        BOOST_CHECK(short_buffer.ptr == buffer.data() + buffer.size() - 1);
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

        // The default base, which every call above passes explicitly. Both
        // sides of the delegation are exercised: int128 renders here, int
        // renders in the standard library.
        auto decimal = std::array<char, xstd::to_chars_max_size<xstd::int128>>{};
        auto const wide = xstd::to_chars(decimal.data(), decimal.data() + decimal.size(), xstd::int128{-42});
        BOOST_CHECK_EQUAL(std::string(decimal.data(), wide.ptr), "-42");
        auto const narrow = xstd::to_chars(decimal.data(), decimal.data() + decimal.size(), 42);
        BOOST_CHECK_EQUAL(std::string(decimal.data(), narrow.ptr), "42");

        // The short-buffer return in the unsigned instantiation, which the
        // coverage gate counts separately from the signed one below.
        auto buffer = std::array<char, xstd::to_chars_max_size<xstd::uint128>>{};
        auto const truncated = xstd::to_chars(
                buffer.data(), buffer.data(), std::numeric_limits<xstd::uint128>::max(), 2);
        BOOST_CHECK(truncated.ec == std::errc::value_too_large);
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

        // The same, in the signed digits instantiation: ptr is left at last
        // rather than pointing into a partial answer.
        auto const wide = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), std::numeric_limits<xstd::int128>::min(), 10);
        BOOST_CHECK(wide.ec == std::errc::value_too_large);
        BOOST_CHECK(wide.ptr == buffer.data() + buffer.size());
}

BOOST_AUTO_TEST_SUITE_END()
