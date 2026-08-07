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

// The largest number of characters to_chars can write for I, at any base it
// accepts. Base 2 is the worst case. For an unsigned type that is exactly one
// character per value bit; for a signed one it is two more, not one: digits
// counts value bits only, so the magnitude of min() is 2^digits and takes
// digits + 1 binary digits, and the sign takes another. At int8_t that is the
// difference between fitting "-10000000" and not. No null terminator: to_chars
// does not write one.
template<integral_like I>
inline constexpr auto to_chars_max_size =
        static_cast<std::size_t>(std::numeric_limits<I>::digits) + (is_signed_like_v<I> ? 2 : 0);

namespace detail {

// A string_view rather than a char array: modernize-avoid-c-arrays is
// enabled, and indexing is all this is for.
inline constexpr auto to_chars_digits = std::string_view{"0123456789abcdefghijklmnopqrstuvwxyz"};

// Named because readability-magic-numbers is enabled for the headers, and
// because a bare 10 in a signature reads as arbitrary until it is called
// decimal.
inline constexpr auto decimal_base = 10;

} // namespace detail

// std::to_chars, widened to every integral-like type: two overloads on one
// name, rather than one function branching on an if constexpr.
//
// Which one a call selects is decided by the constraints alone. This one is
// constrained on nothing beyond integral_like, and the one below adds a
// requires-expression on top of it; a conjunction subsumes its left operand, so
// wherever both are viable the delegating one is more constrained and wins
// partial ordering. Spelling this one's constraint as the negation would work
// too, but a negated atomic constraint does not subsume, so exclusivity and
// exhaustiveness would become an invariant to maintain across two edits rather
// than a property of the constraints. As written there is no gap and no
// ambiguity: every integral-like type matches this overload, and the ones the
// standard library covers match both.
//
// This one comes first for a reason outside the language. gcov's text output
// names only the first group of functions sharing a start line in a file, and
// gcovr keys its cross-translation-unit merge on those names: whatever is left
// unnamed merges with nothing. The two bodies are not symmetric in what that
// costs. This one contains a line no other translation unit can reach - the
// short-buffer return, which only the charconv test provokes - so its records
// have to merge with the copies emitted wherever else it is instantiated, such
// as the div_t formatter on a standard library without p2286. Every line of the
// delegating body runs in every instantiation that exists at all, so leaving
// that one unnamed costs nothing. Ordered the other way, the file reads 98%
// covered and the gate fails.
//
// Same output as the standard's, for the types it does not cover: integer-class
// types everywhere, and the built-in 128-bit types in the strict dialect this
// library compiles in. Digits are counted first so the result can be written
// forwards into the caller's range, which is the interface the standard
// specifies: no null terminator, and ptr one past the last character written.
//
// The magnitude is never formed. Negating the minimum value has no
// representation, and an integer-class type need not offer a wider one to
// borrow, so the remainder is taken on the negative value and the digit's sign
// flipped. That also means no make_unsigned_like is needed, which an
// integer-class type is only required to have if its author supplied one.
template<integral_like I>
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = detail::decimal_base) noexcept
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);

        auto const radix = static_cast<I>(base);

        auto count = std::size_t{0};
        for (auto rest = value;; rest = static_cast<I>(rest / radix)) {
                ++count;
                if (rest / radix == I{0}) {
                        break;
                }
        }

        // maybe_unused because every read of it below is in an if constexpr
        // branch an unsigned instantiation discards, leaving the variable set
        // and never used there.
        [[maybe_unused]] auto negative = false;
        if constexpr (is_signed_like_v<I>) {
                negative = value < I{0};
                count += negative ? std::size_t{1} : std::size_t{0};
        }

        if (std::cmp_less(last - first, count)) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        auto* out = first + count;
        for (auto rest = value;; rest = static_cast<I>(rest / radix)) {
                auto digit = static_cast<int>(rest % radix);
                // Both sign tests are under if constexpr rather than a plain
                // if: an unsigned I can never be negative, so a runtime test
                // would emit a line and a branch that its instantiation cannot
                // reach - and the coverage gate counts both per instantiation.
                if constexpr (is_signed_like_v<I>) {
                        // The remainder of a negative value is negative, and
                        // the sign is written once, below.
                        if (negative) {
                                digit = -digit;
                        }
                }
                *--out = detail::to_chars_digits[static_cast<std::size_t>(digit)];
                if (rest / radix == I{0}) {
                        break;
                }
        }
        if constexpr (is_signed_like_v<I>) {
                if (negative) {
                        *--out = '-';
                }
        }
        return {.ptr = first + count, .ec = std::errc{}};
}

// The more constrained of the two, selected wherever the standard library
// covers I. It is that call and nothing more, so the tuned implementation and
// its format-spec behavior are what a caller gets.
//
// The constraint is the call itself: there is no standard concept for "the
// standard library converts this type", so the requires-expression names
// std::to_chars directly rather than being wrapped in a helper concept. Two
// ways it can be unsatisfied, and it has to survive both:
//
// - No overload at all, for an integer-class type. A clean deduction failure.
// - An *ambiguous* overload, which is what libstdc++ gives for __int128 in
//   strict mode: the public overload set is a set of non-template functions
//   generated per built-in width, and the 128-bit pair is gated on
//   __GLIBCXX_TYPE_INT_N_0, which the compiler only predefines outside
//   __STRICT_ANSI__. Without it the argument converts equally well to every
//   width. Ambiguity is still overload resolution failing in the immediate
//   context, so the requirement is unsatisfied rather than ill-formed.
//
// Both of those are answers rather than errors only because I is this
// template's own parameter, so the expression stays dependent until the
// constraint is checked. The same requires-expression written at a concrete
// type - as the tests want, to assert which path a type takes - has to be a
// named concept there, for exactly that reason.
//
// std::to_chars(bool) is deleted, so bool is unsatisfied here and is deleted
// below rather than silently picking up the digits overload.
template<integral_like I>
        requires requires (char* p, I value, int base) {
                { std::to_chars(p, p, value, base) } -> std::same_as<std::to_chars_result>;
        }
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = detail::decimal_base) noexcept
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);
        return std::to_chars(first, last, value, base);
}

// Deleted for the same reason the standard deletes it: bool is integral-like,
// so an unconstrained call would silently render true as "1" rather than say
// that the caller almost certainly meant something else.
auto to_chars(char*, char*, bool, int = detail::decimal_base) -> std::to_chars_result = delete;

} // namespace xstd

#endif // XSTD_CHARCONV_TO_CHARS_HPP
