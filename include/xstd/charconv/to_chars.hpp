//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CHARCONV_TO_CHARS_HPP
#define XSTD_CHARCONV_TO_CHARS_HPP

#include <xstd/concepts/integral_like.hpp>     // integral_like
#include <xstd/type_traits/is_signed_like.hpp> // is_signed_like_v
#include <cassert>                             // assert
#include <charconv>                            // to_chars, to_chars_result
#include <concepts>                            // same_as
#include <cstddef>                             // size_t
#include <limits>                              // numeric_limits
#include <string_view>                         // string_view
#include <system_error>                        // errc
#include <utility>                             // cmp_less

namespace xstd {

// The largest number of characters to_chars can write for T, at any base it
// accepts. Base 2 is the worst case. For an unsigned type that is exactly one
// character per value bit; for a signed one it is two more, not one: digits
// counts value bits only, so the magnitude of min() is 2^digits and takes
// digits + 1 binary digits, and the sign takes another. At int8_t that is the
// difference between fitting "-10000000" and not. No null terminator: to_chars
// does not write one.
template<integral_like T>
inline constexpr auto to_chars_max_size =
        static_cast<std::size_t>(std::numeric_limits<T>::digits) + (is_signed_like_v<T> ? 2 : 0);

namespace detail {

// A string_view rather than a char array: modernize-avoid-c-arrays is
// enabled, and indexing is all this is for.
inline constexpr auto to_chars_digits = std::string_view{"0123456789abcdefghijklmnopqrstuvwxyz"};

// Named because readability-magic-numbers is enabled for the headers, and
// because a bare 10 in a signature reads as arbitrary until it is called
// decimal.
inline constexpr auto decimal_base = 10;

// Is the standard's to_chars usable for T? Two ways it can fail to be, and the
// concept has to survive both:
//
// - No overload at all, for an integer-class type. A clean deduction failure.
// - An *ambiguous* overload, which is what libstdc++ gives for __int128 in
//   strict mode: the public overload set is a set of non-template functions
//   generated per built-in width, and the 128-bit pair is gated on
//   __GLIBCXX_TYPE_INT_N_0, which the compiler only predefines outside
//   __STRICT_ANSI__. Without it the argument converts equally well to every
//   width. Ambiguity is still overload resolution failing in the immediate
//   context, so the requires-expression is false rather than ill-formed - as
//   long as T stays dependent, which is why this is a named concept.
//
// std::to_chars(bool) is deleted, so bool lands here as false and is deleted
// below rather than silently picking up the fallback.
template<class T>
concept has_std_to_chars = requires (char* p, T value, int base) {
        { std::to_chars(p, p, value, base) } -> std::same_as<std::to_chars_result>;
};

// Same output as the standard's, for the types it does not cover. Digits are
// counted first so the result can be written forwards into the caller's range,
// which is the interface the standard specifies: no null terminator, and ptr
// one past the last character written.
//
// The magnitude is never formed. Negating the minimum value has no
// representation, and an integer-class type need not offer a wider one to
// borrow, so the remainder is taken on the negative value and the digit's sign
// flipped. That also means no make_unsigned_like is needed, which an
// integer-class type is only required to have if its author supplied one.
template<integral_like T>
[[nodiscard]] constexpr auto to_chars_fallback(char* first, char* last, T value, int base) noexcept
        -> std::to_chars_result
{
        auto const radix = static_cast<T>(base);

        auto count = std::size_t{0};
        for (auto rest = value;; rest = static_cast<T>(rest / radix)) {
                ++count;
                if (rest / radix == T{0}) {
                        break;
                }
        }

        // maybe_unused because every read of it below is in an if constexpr
        // branch an unsigned instantiation discards, leaving the variable set
        // and never used there.
        [[maybe_unused]] auto negative = false;
        if constexpr (is_signed_like_v<T>) {
                negative = value < T{0};
                count += negative ? std::size_t{1} : std::size_t{0};
        }

        if (std::cmp_less(last - first, count)) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        auto* out = first + count;
        for (auto rest = value;; rest = static_cast<T>(rest / radix)) {
                auto digit = static_cast<int>(rest % radix);
                // Both sign tests are under if constexpr rather than a plain
                // if: an unsigned T can never be negative, so a runtime test
                // would emit a line and a branch that its instantiation cannot
                // reach - and the coverage gate counts both per instantiation.
                if constexpr (is_signed_like_v<T>) {
                        // The remainder of a negative value is negative, and
                        // the sign is written once, below.
                        if (negative) {
                                digit = -digit;
                        }
                }
                *--out = to_chars_digits[static_cast<std::size_t>(digit)];
                if (rest / radix == T{0}) {
                        break;
                }
        }
        if constexpr (is_signed_like_v<T>) {
                if (negative) {
                        *--out = '-';
                }
        }
        return {.ptr = first + count, .ec = std::errc{}};
}

} // namespace detail

// std::to_chars, widened to every integral-like type. Where the standard
// library already covers T this is that call and nothing more, so the tuned
// implementation and its format-spec behavior are what a caller gets. Where it
// does not - integer-class types everywhere, and the built-in 128-bit types in
// the strict dialect this library compiles in - the digits are produced here to
// the same specification.
template<integral_like T>
[[nodiscard]] constexpr auto to_chars(char* first, char* last, T value, int base = detail::decimal_base) noexcept
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);
        if constexpr (detail::has_std_to_chars<T>) {
                return std::to_chars(first, last, value, base);
        } else {
                return detail::to_chars_fallback(first, last, value, base);
        }
}

// Deleted for the same reason the standard deletes it: bool is integral-like,
// so an unconstrained call would silently render true as "1" rather than say
// that the caller almost certainly meant something else.
auto to_chars(char*, char*, bool, int = detail::decimal_base) -> std::to_chars_result = delete;

} // namespace xstd

#endif // XSTD_CHARCONV_TO_CHARS_HPP
