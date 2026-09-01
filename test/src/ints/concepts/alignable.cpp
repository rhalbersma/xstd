//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/alignable.hpp>        // alignable, nothrow_alignable
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                                 // strong_ordering
#include <concepts>                                // constructible_from
#include <cstddef>                                 // size_t
#include <cstdint>                                 // exact-width integer types, uintptr_t
#include <limits>                                  // numeric_limits

#ifdef __SIZEOF_INT128__
#define XSTD_TEST_HAS_INT128
#endif

#if __has_include(<absl/numeric/int128.h>)
#define XSTD_TEST_HAS_ABSL_INT128
#include <absl/numeric/int128.h> // uint128
#endif

namespace {

// Alignable without being an integer: no division, no increments, no compound assignment.
struct light
{
        std::uint64_t v = 0;

        constexpr light() = default;
        constexpr light(std::size_t n) : v(n) {}

        [[nodiscard]] constexpr operator std::size_t() const
        {
                return v;
        }

        [[nodiscard]] constexpr auto operator<=>(light const&) const -> std::strong_ordering = default;
        [[nodiscard]] constexpr auto operator==(light const&) const -> bool = default;

        [[nodiscard]] constexpr auto operator+(light o) const
                -> light
        {
                return light(v + o.v);
        }

        [[nodiscard]] constexpr auto operator-(light o) const
                -> light
        {
                return light(v - o.v);
        }

        [[nodiscard]] constexpr auto operator&(light o) const
                -> light
        {
                return light(v & o.v);
        }
};

} // namespace

namespace xstd {

// What alignable asks of the type, borrowed whole from the integer it is a thin wrapper over.
template<>
struct numeric_limits<light> : std::numeric_limits<std::uint64_t>
{};

} // namespace xstd

BOOST_AUTO_TEST_SUITE(Concepts)

BOOST_AUTO_TEST_CASE(AlignableAdmitsTheUnsignedIntegers)
{
        static_assert(xstd::alignable<unsigned char>);
        static_assert(xstd::alignable<unsigned short>);
        static_assert(xstd::alignable<unsigned>);
        static_assert(xstd::alignable<unsigned long>);
        static_assert(xstd::alignable<unsigned long long>);
        static_assert(xstd::alignable<std::size_t>);
        static_assert(xstd::alignable<std::uintptr_t>);
        static_assert(xstd::alignable<std::uint8_t>);
        static_assert(xstd::alignable<std::uint64_t>);
#ifdef XSTD_TEST_HAS_INT128
        static_assert(xstd::alignable<__uint128_t>);
#endif
#ifdef __BITINT_MAXWIDTH__
        static_assert(xstd::alignable<unsigned _BitInt(8)>);
        static_assert(xstd::alignable<unsigned _BitInt(24)>);
        static_assert(xstd::alignable<unsigned _BitInt(64)>);
#endif
        BOOST_CHECK(true);
}

// Nothing is excluded by name: each of these fails a clause it cannot satisfy.
BOOST_AUTO_TEST_CASE(AndRejectsTheRestOnTheirMerits)
{
        // Signed, so a wrapped sum would be undefined rather than a value the precondition can ask about.
        static_assert(not xstd::alignable<signed char>);
        static_assert(not xstd::alignable<short>);
        static_assert(not xstd::alignable<int>);
        static_assert(not xstd::alignable<long long>);
#ifdef XSTD_TEST_HAS_INT128
        static_assert(not xstd::alignable<__int128_t>);
#endif
#ifdef __BITINT_MAXWIDTH__
        static_assert(not xstd::alignable<signed _BitInt(24)>);
#endif
        // One bit wide, which is what the digits clause is for: bool's one plus one is still one.
        static_assert(not xstd::alignable<bool>);
        static_assert(xstd::numeric_limits<bool>::digits == 1);
        // Not an integer, and for the pointers not specialized at all, which is not_pointer for free.
        static_assert(not xstd::alignable<double>);
        static_assert(not xstd::alignable<void*>);
        static_assert(not xstd::alignable<std::uint8_t*>);
        // Nothing at all.
        struct udt
        {};
        static_assert(not xstd::alignable<udt>);
        static_assert(not xstd::alignable<void>);
        BOOST_CHECK(true);
}

// The point of the concept: it is integer_class's opening clauses and then it stops.
BOOST_AUTO_TEST_CASE(IsIntegerLight)
{
#define XSTD_TEST_REFINES(T) \
        static_assert(not xstd::unsigned_integer<T> or xstd::alignable<T>); \
        static_assert(not xstd::signed_integer<T> or not xstd::alignable<T>); \
        static_assert(not xstd::alignable<T> or std::constructible_from<std::size_t, T>)
        XSTD_TEST_REFINES(unsigned char);
        XSTD_TEST_REFINES(unsigned);
        XSTD_TEST_REFINES(unsigned long long);
        XSTD_TEST_REFINES(std::size_t);
        XSTD_TEST_REFINES(std::uint8_t);
        XSTD_TEST_REFINES(signed char);
        XSTD_TEST_REFINES(int);
        XSTD_TEST_REFINES(bool);
#ifdef XSTD_TEST_HAS_INT128
        XSTD_TEST_REFINES(__uint128_t);
        XSTD_TEST_REFINES(__int128_t);
#endif
#ifdef __BITINT_MAXWIDTH__
        XSTD_TEST_REFINES(unsigned _BitInt(24));
        XSTD_TEST_REFINES(signed _BitInt(24));
#endif
#undef XSTD_TEST_REFINES

        // And the converse fails, which is what makes it light rather than a synonym for unsigned_integer.
        static_assert(xstd::alignable<light>);
        static_assert(not xstd::unsigned_integer<light>);
        BOOST_CHECK(light{} == light{0});
        BOOST_CHECK((light{6} + light{1}) == light{7});
        BOOST_CHECK((light{6} - light{1}) == light{5});
        BOOST_CHECK((light{6} & light{3}) == light{2});
        BOOST_CHECK(light{6} >= light{5});
        BOOST_CHECK_EQUAL(static_cast<std::size_t>(light{6}), 6UZ);
        BOOST_CHECK(true);
}

// Only visible on a class type without noexcept, which Abseil is: it declares noexcept nowhere.
BOOST_AUTO_TEST_CASE(NothrowAlignableRefinesIt)
{
        static_assert(xstd::nothrow_alignable<unsigned char>);
        static_assert(xstd::nothrow_alignable<std::size_t>);
#ifdef XSTD_TEST_HAS_INT128
        static_assert(xstd::nothrow_alignable<__uint128_t>);
#endif
#ifdef XSTD_TEST_HAS_ABSL_INT128
        static_assert(xstd::alignable<absl::uint128>);
        static_assert(not xstd::nothrow_alignable<absl::uint128>);
        // The way back out, which is what the address round trip needs of a type wider than a pointer.
        static_assert(std::constructible_from<std::size_t, absl::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
