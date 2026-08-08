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
#include <utility>                             // cmp_equal, cmp_less

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
//
// Biased rather than starting at '0', so that one lookup serves a remainder of
// either sign. A remainder is always in (-base, base) and base is at most 36,
// so the index is to_chars_digits_zero + [-35, 35]: every position of this
// table and no other. That is what lets the digit loop below carry no test on
// the sign at all - a runtime one would be a branch an unsigned instantiation
// could never take, and the coverage gate counts branches per instantiation.
inline constexpr auto to_chars_digits = std::string_view{"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"};

// Where '0' sits in it. An int, so that the index arithmetic stays in a signed
// type rather than leaning on modular wraparound to bring a negative digit
// back into range.
inline constexpr auto to_chars_digits_zero = 35;
static_assert(std::cmp_equal(to_chars_digits.size(), 2 * to_chars_digits_zero + 1));

} // namespace detail

// std::to_chars, widened to every integral-like type: two overloads on one
// name, rather than one function branching on an if constexpr.
//
// The default base is the literal 10, the way [charconv.to.chars] spells it,
// rather than a named constant. A caller reads this signature against the
// standard's, so the two should differ in nothing that is visible there.
// readability-magic-numbers is enabled for the headers and does fire on a
// default argument, so each of the three declarations carries a NOLINTNEXTLINE
// for it - the narrowest suppression there is, one check on one line, rather
// than adding 10 to the check's ignored values and losing it everywhere else in
// the library.
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
// borrow, so the remainder is taken on the negative value and its sign is
// absorbed by the digit table's bias. That also means no make_unsigned_like is
// needed, which an integer-class type is only required to have if its author
// supplied one.
template<integral_like I>
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10) noexcept
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);

        auto const radix = static_cast<I>(base);

        // Not written inline below: value < I{0} on an unsigned type is
        // -Wtype-limits, which this library builds with -Werror, and only the
        // discarded branch of an if constexpr keeps that comparison from being
        // instantiated at all. The return type is pinned rather than deduced
        // because an integer-class type's relational operators need only be
        // boolean-testable, and a proxy deduced here would outlive the I{0} it
        // was formed from.
        auto const negative = [value] -> bool {
                if constexpr (is_signed_like_v<I>) {
                        return value < I{0};
                } else {
                        return false;
                }
        }();

        auto count = std::size_t{0};
        for (auto rest = value;; rest = static_cast<I>(rest / radix)) {
                ++count;
                if (rest / radix == I{0}) {
                        break;
                }
        }
        // Converted rather than selected with a conditional: a conditional is a
        // branch gcov records per instantiation, and an unsigned one has no way
        // to reach its other side.
        count += static_cast<std::size_t>(negative);

        if (std::cmp_less(last - first, count)) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        auto* out = first + count;
        for (auto rest = value;; rest = static_cast<I>(rest / radix)) {
                // The remainder of a negative value is negative, which the
                // table's bias takes care of; the sign itself is written once,
                // below.
                auto const digit = static_cast<int>(rest % radix);
                *--out = detail::to_chars_digits[static_cast<std::size_t>(detail::to_chars_digits_zero + digit)];
                if (rest / radix == I{0}) {
                        break;
                }
        }
        // The one place the sign still has to be a test rather than an index or
        // a conversion, and so the one that stays under an if constexpr for the
        // reason given above.
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
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10) noexcept
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);
        return std::to_chars(first, last, value, base);
}

// Deleted for the same reason the standard deletes it: bool is integral-like,
// so an unconstrained call would silently render true as "1" rather than say
// that the caller almost certainly meant something else.
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
auto to_chars(char*, char*, bool, int = 10) -> std::to_chars_result = delete;

} // namespace xstd

#endif // XSTD_CHARCONV_TO_CHARS_HPP
