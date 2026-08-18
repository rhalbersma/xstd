//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BIT_PRECISE_HPP
#define XSTD_TEST_BIT_PRECISE_HPP

// C23's bit-precise integers, which in C++ only Clang has, and whose ceiling it names here.
#ifdef __BITINT_MAXWIDTH__

#include <xstd/test/bit_integer.hpp>          // bit_integer, storage_limits
#include <xstd/type_traits/make_signed.hpp>   // make_signed
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned
#include <cstddef>                            // size_t
#include <type_traits>                        // type_identity

#define XSTD_TEST_HAS_BIT_PRECISE

// Dividing wider than 64 bits calls compiler-rt, which clang-cl does not link: __udivti3.
#ifdef _MSC_VER
#define XSTD_TEST_BIT_PRECISE_MAX 64
#elif __BITINT_MAXWIDTH__ >= 256
#define XSTD_TEST_BIT_PRECISE_MAX 256
#else
#define XSTD_TEST_BIT_PRECISE_MAX 128
#endif

namespace xstd::test {

// A width std cannot be told about, [namespace.std]/2 reserving that to program-defined types.
template<std::size_t N>
struct storage_limits<unsigned _BitInt(N)>
{
        using type = unsigned _BitInt(N);

        static constexpr auto is_specialized = true;
        static constexpr auto is_signed = false;
        static constexpr auto is_integer = true;
        static constexpr auto is_exact = true;
        static constexpr auto is_bounded = true;
        static constexpr auto is_modulo = true;
        static constexpr auto digits = static_cast<int>(N);
        static constexpr auto radix = 2;

        [[nodiscard]] static constexpr auto min() -> type
        {
                return type{0};
        }
        [[nodiscard]] static constexpr auto max() -> type
        {
                return static_cast<type>(~type{0});
        }
        [[nodiscard]] static constexpr auto lowest() -> type
        {
                return min();
        }
};

// One value bit fewer, the sign taking the other; N of 1 is why no signed list starts there.
template<std::size_t N>
struct storage_limits<signed _BitInt(N)>
{
        using type = signed _BitInt(N);
        using unsigned_type = unsigned _BitInt(N);

        static constexpr auto is_specialized = true;
        static constexpr auto is_signed = true;
        static constexpr auto is_integer = true;
        static constexpr auto is_exact = true;
        static constexpr auto is_bounded = true;
        static constexpr auto is_modulo = false;
        static constexpr auto digits = static_cast<int>(N) - 1;
        static constexpr auto radix = 2;

        [[nodiscard]] static constexpr auto max() -> type
        {
                return static_cast<type>(static_cast<unsigned_type>(~unsigned_type{0}) >> 1U);
        }
        [[nodiscard]] static constexpr auto min() -> type
        {
                return static_cast<type>(-max() - 1);
        }
        [[nodiscard]] static constexpr auto lowest() -> type
        {
                return min();
        }
};

// Wrapped rather than used raw: is_integral is false for these, so integer_like refuses them.
template<std::size_t N>
using bit_uint = bit_integer<unsigned _BitInt(N)>;

// C23 sets the signed minimum at two bits, so only the unsigned list starts at one.
template<std::size_t N>
using bit_int = bit_integer<signed _BitInt(N)>;

} // namespace xstd::test

// The pair, one specialization per direction; each type is its own the other way round.
template<std::size_t N>
struct xstd::make_unsigned<xstd::test::bit_int<N>> : std::type_identity<xstd::test::bit_uint<N>>
{};

// Constrained where C23's two-bit signed minimum leaves a one-bit unsigned with no counterpart.
template<std::size_t N>
        requires (N >= 2)
struct xstd::make_signed<xstd::test::bit_uint<N>> : std::type_identity<xstd::test::bit_int<N>>
{};

#endif // __BITINT_MAXWIDTH__

#endif // XSTD_TEST_BIT_PRECISE_HPP
