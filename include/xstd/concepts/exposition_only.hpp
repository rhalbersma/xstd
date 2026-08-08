//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
#define XSTD_CONCEPTS_EXPOSITION_ONLY_HPP

#include <compare>     // strong_ordering
#include <concepts>    // constructible_from, integral, regular, same_as, three_way_comparable
#include <cstddef>     // size_t
#include <limits>      // numeric_limits
#include <type_traits> // remove_cv_t

// Internal concepts used to define xstd's public numeric traits and concepts.
namespace xstd::exposition_only {

// Structural form of the integer-class requirements in [iterator.concept.winc],
// stated as the subclause states them: for an object of the type, and so for a
// type carrying no cv-qualification. integer_class_type below is the spelling
// to use.
//
// None of these expressions is required to be noexcept, because the subclause
// never says so: the word does not appear in it. The only thing it says about
// throwing is /6, that conversions "do not exit via an exception" - a statement
// about what happens at run time, not about how the conversion is declared, and
// so not something a requires-expression can check. Requiring the specifier
// anyway would narrow this to integer-class types whose authors annotated them,
// which is a different and much smaller set: absl::uint128, to name the obvious
// one, does not use the word once in its header, though its two uint64_t halves
// have no way to throw. What the annotations are worth is decided where it can
// be acted on - the arithmetic surface propagates them, see the public
// xstd::nothrow_arithmetic, which is these same requirements over const
// operands with the specifier asked of each.
template<class I>
concept unqualified_integer_class_type =
        // The subclause never says an integer-class type is not an integral
        // one, but /3 leaves no room for it: the width of an integer-class type
        // "is greater than that of every integral type of the same signedness",
        // and no integral type manages that against itself. A structural check
        // cannot derive the width clause - "every integral type" reaches the
        // extended ones, so any bound written here would be the platform's
        // rather than the standard's - so the consequence is spelled instead.
        //
        // It earns the line. Drop it and int, short, char and long satisfy
        // every requirement below, and this concept answers true for four types
        // /3 rules out. (bool does not, ++a on it having gone in C++17; bool
        // reaches integral_like through std::integral, not through here.)
        //
        // What it does not turn away is note 1's case, "an integer-class type
        // is not necessarily a class type". __int128 is not a class type, and
        // with CMAKE_CXX_EXTENSIONS OFF - so -std=c++23, not -std=gnu++23 -
        // libstdc++ does not answer std::integral for it either, which is
        // exactly how xstd::int128 is admitted there: right here, on its own
        // operations. Under -std=gnu++23, or under libc++ at either dialect,
        // the same type answers std::integral and takes the other branch of
        // integral_like. That the disjunction gives one answer across all
        // three is the point of having two branches.
        (not std::integral<I>) and
        requires { sizeof(I); } and
        // [iterator.concept.winc] first specifies conversions between integral
        // and integer-class types.
        std::constructible_from<I, int> and
        // It then specifies increment/decrement, unary operators, compound
        // assignment, and non-assignment binary operators, in that order.
        requires (I a) {
                { a++ } -> std::same_as<I>;
                { a-- } -> std::same_as<I>;
                { ++a } -> std::same_as<I&>;
                { --a } -> std::same_as<I&>;
        } and
        requires (I const a) {
                { static_cast<I>(+a) } -> std::same_as<I>;
                { static_cast<I>(-a) } -> std::same_as<I>;
                { static_cast<I>(~a) } -> std::same_as<I>;
                // bool exactly, not merely convertible to it. /7.3 ranges over
                // unary-operator, which is `* & + - ! ~`, excludes `&` by name
                // and leaves `*` with nothing an integer can apply it to - so
                // these four are its whole scope - and then pins the result in
                // one sentence: "if @x has type bool, so too does @a; if @x has
                // type B(I), then @a has type I". The first half of that
                // sentence is there for `!` alone, the one operator of the four
                // whose result on an integer is not the integer type again.
                // convertible_to<bool> would leave it with nothing to say.
                { not a } -> std::same_as<bool>;
        } and
        requires (I a, I const b) {
                { a += b } -> std::same_as<I&>;
                { a -= b } -> std::same_as<I&>;
                { a *= b } -> std::same_as<I&>;
                { a /= b } -> std::same_as<I&>;
                { a %= b } -> std::same_as<I&>;
                { a &= b } -> std::same_as<I&>;
                { a |= b } -> std::same_as<I&>;
                { a ^= b } -> std::same_as<I&>;
        } and
        requires (I a, std::size_t const n) {
                { a <<= n } -> std::same_as<I&>;
                { a >>= n } -> std::same_as<I&>;
        } and
        requires (I const a, I const b) {
                { static_cast<I>(a + b) } -> std::same_as<I>;
                { static_cast<I>(a - b) } -> std::same_as<I>;
                { static_cast<I>(a * b) } -> std::same_as<I>;
                { static_cast<I>(a / b) } -> std::same_as<I>;
                { static_cast<I>(a % b) } -> std::same_as<I>;
                { static_cast<I>(a & b) } -> std::same_as<I>;
                { static_cast<I>(a | b) } -> std::same_as<I>;
                { static_cast<I>(a ^ b) } -> std::same_as<I>;
        } and
        requires (I const a, std::size_t const n) {
                { static_cast<I>(a << n) } -> std::same_as<I>;
                { static_cast<I>(a >> n) } -> std::same_as<I>;
        } and
        requires (I const a, I const b) {
                // Pinned for the same reason, by the last clause of /7.6:
                // `a @ b` has whatever type `x @ y` has once that type is
                // neither B(I) nor B(I2), which for the six comparisons is bool
                // and for <=> is strong_ordering.
                //
                // All seven are spelled out because the regularity and ordering
                // below stop short of six of them. std::equality_comparable
                // asks that `a == b` and `a != b` be boolean-testable, and the
                // four relational operators reach this concept only through
                // std::three_way_comparable, which asks no more of them either.
                // Boolean-testable is weaker than /7.6: it admits a proxy the
                // subclause does not describe. Only the <=> line is a
                // restatement - three_way_comparable<I, strong_ordering>
                // already forces that exact type through compares-as - and it
                // is kept because /7.6 is where the subclause says so.
                { a == b } -> std::same_as<bool>;
                { a != b } -> std::same_as<bool>;
                { a < b } -> std::same_as<bool>;
                { a > b } -> std::same_as<bool>;
                { a <= b } -> std::same_as<bool>;
                { a >= b } -> std::same_as<bool>;
                { a <=> b } -> std::same_as<std::strong_ordering>;
        } and
        // Finally come regularity and ordering, value-initialization, and
        // numeric_limits.
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        requires {
                { static_cast<I>(0) } -> std::same_as<I>;
        } and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

// The same requirements, asked of a type that may be cv-qualified.
//
// /11 speaks of "every (possibly cv-qualified) integer-class type", and the
// standard's own category traits answer the same for int as for int const, so
// the widening has to as well. It cannot do that by asking the requirements
// above of a qualified type directly: they are stated for an object that can be
// assigned and incremented, and `++a`, `a += b` and std::regular all fail on a
// const one. Without this, integral_like admitted `int const` and turned away
// `absl::uint128 const` - a difference between the two branches that nothing in
// the subclause asks for. Stripped once here rather than in each requirement.
template<class I>
concept integer_class_type = unqualified_integer_class_type<std::remove_cv_t<I>>;

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
