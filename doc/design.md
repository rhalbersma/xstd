# Design notes

## Purpose

xstd is a header-only collection of small C++ standard-library extensions.
There is no single proposal behind it: it collects facilities suggested by
existing practice that can be implemented portably, with a standard-library
interface. The baseline is C++23, and may move to C++26 once that is common
across the tested toolchains. Consumers need no third-party dependencies.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents it.
- **Generalize integer support.** The standard integral concepts and traits are
  closed over built-in types; xstd's `_like` counterparts also accept
  integer-class types, on their behavior and `std::numeric_limits`. They are
  named and defined after [iterator.concept.winc]'s *integer-like*, which
  excludes cv `bool`.
- **Keep metaprogramming small.** `specialization_of`, `empty_type` and
  `conditional_data_member_t` solve local problems without a framework.
- **Make semantics explicit.** The three division functions name their rounding
  convention rather than hiding it behind `/` and `%`.
- **Stay modular and dependency-free.** Linking `xstd::xstd` adds include paths
  and the C++23 requirement, but no runtime library or transitive package.

## API shape

### Integer-like types

`integer_like`, `signed_integer_like` and `unsigned_integer_like` widen the
standard concepts to structurally recognized integer-class types, and the `_like`
traits follow the same rule. The subclause names exactly one exposition-only
concept, *is-integer-like*, and introduces *integer-class type* as a term instead
— "a set of implementation-defined types that behave as integer types do" (/2).
`integer_class` is this library's structural reading of that term, which is why it
carries a name of its own rather than a hidden one: the term is normative, not
exposition-only. It reads the term and nothing more. `integer_like` is that
concept or `std::integral`, and one thing more: that the type be one of a
signed/unsigned pair. Built-in integers need no customization; a
user-defined pair supplies the opposite `make_signed` and `make_unsigned`
specializations, and a type arriving without them is not an integer-class type
here, so every function turns it away by the constraint it already had.

The widening is cv-transparent on both branches. That takes doing only on the
integer-class one: [iterator.concept.winc] states its requirements for an object
of the type, so a `const` type fails at `++a`, at `a += b` and at `std::regular`.
Nothing in the subclause wants that difference — /11 speaks of "every (possibly
cv-qualified) integer-class type" — so the qualification comes off once, in a
defaulted second template parameter that binds the type the body is written over.
`nothrow_integer_operators` uses the same spelling: `const` would have survived
unaided, every row being stated over a `const` operand, but `volatile` splits the
two branches, a built-in answering a volatile operand on the language's own
operators where a class type's `const` members answer none of them.

Where the subclause constrains a result type the concept asks for that type, with
one exception: /7.6's `bool` for the comparisons is left to `std::regular` and
`std::three_way_comparable`, which ask only for boolean-testable results. Nothing
here can tell the difference, every use being a contextual conversion. The
language draws the line in one place anyway — a rewritten `!=` needs its
`operator==` to return exactly `bool` ([over.match.oper]) — so a type whose
equality returns a proxy must write `!=` out, and then the concept admits it.

That the concept asks for `I` exactly, rather than for something a `static_cast<I>`
could reach, is pinned by a pair of fixtures in `test/include/xstd/test/` that are
one class template at one storage type, differing in /7.6 alone: the conforming
one is asserted to satisfy `integer_class` and the proxy-returning one to
fail it. Both halves are the test. A failing assertion on its own would hold just
as well for a fixture that had drifted out of conformance somewhere else entirely,
and would go on holding after the clause it was written for had been relaxed.

A template parameter is named for its concept: `I` under `integer_like`, `S`
under `signed_integer_like`. The letters carry the constraint into the body,
where `S{-1}` reads as something the type can hold and `I{-1}` would not — which
is the line `div` takes care over, its `static_cast<I>(-1)` naming `max()` rather
than a value below zero.

The arithmetic functions use one constrained template per operation, over
`integer_like` rather than its signed half, covering every integer-like width of
either signedness without families such as `abs`, `labs`, `llabs` and `imaxabs`.
A function returns the argument type rather than a promoted one, and a
two-argument function requires both arguments to have the same type.

Over an unsigned type four of the six are the function they already were and one
is the identity, but each says so rather than leaving it to a dead adjustment.
`abs` and `unsigned_abs` share a shape, which is what says they coincide there
and differ only in return type. `euclidean_div` and `floored_div` return
`xstd::div`'s answer outright: a truncated remainder carries the numerator's
sign, so an unsigned one is already nonnegative and already agrees with its
denominator. Left to the adjustment, the floored case would hold only by way of
the nonzero-denominator precondition above it, which is a proof a reader has to
reconstruct. Neither unsigned branch asserts its convention's postcondition, an
assertion there being unable to fail: `sign` branches on the same
`is_unsigned_v` these do, so over an unsigned type it answers 0 or 1 by
construction rather than by anything the division established — it could not
even catch a `numeric_limits` specialization that lied, being misled by the same
predicate. What is left to check, that the remainder is smaller than the
denominator, `xstd::div` asserts already.

`sign` branches for a different reason: the difference of two
comparisons is correct as written, but the second is one the answer can never
depend on, and for an integer-class type a comparison is a call — and `sign` is
on the path of `div`'s postconditions and `floored_div`'s adjustment.

`bool` is excluded because the subclause excludes it. [iterator.concept.winc]/1
opens "a type `I` **other than cv `bool`** is integer-like if it models
`integral<I>` or if it is an integer-class type", so the exclusion is the first
thing the definition says, and `integer_like` says it in the same place. The
`remove_cv_t` is the subclause's "cv" and nothing more: `is_integral_v<const
bool>` is `true`, so a bare `same_as<I, bool>` would let `const bool` through.

The reason the subclause is right is worth writing down, because it is the reason
the six deleted overloads used to give six different ways. `bool` is a truth
value, not the one-bit unsigned integer a `uint1_t` would be, had the language
one. Such a type would be modular, and `bool` is not. Converting to it normalizes
instead of wrapping — `bool(2)` is `true` where `uint8_t(256)` is `0`. Its
operators promote before they compute, so `true + true` is `2` and not `0`, and
`-`, `~`, `&` and `<<` all hand back an `int`. Its increment was removed in C++17
and it never had a decrement, leaving it the one integral type that steps
nowhere. So `abs(true)` would answer `true` and `sign(true)` would answer `1`:
not wrong about a truth value, but answers to questions that were only ever about
numbers.

Those deletions are gone. A type the concept turns away needs no overload to
turn it away twice, and six of them said in six wordings what the definition now
says once. `to_chars` keeps its one, because `std::to_chars` keeps its one.

The character types stay, and that is the subclause's answer too: they are
integral and they are not `bool`. `<charconv>` agrees — `std::to_chars` accepts
every one of them and prints it numerically — so narrowing here would make
`xstd::to_chars` refuse calls the facility it widens accepts. A stricter line
exists and is proposed: [p3701](https://wg21.link/p3701) would add
`std::integer`, excluding `bool` and every character type. If it lands, the
arithmetic surface may want that set and `to_chars` will still want this one.
It cannot simply be adopted: `std::integer` is closed over built-in types, so no
integer-class type can satisfy it, and its `cv-unqualified` conjunct rejects the
`const` operands these concepts deliberately accept.

An integer-class type is one of a pair here, which is more than
[iterator.concept.winc] asks and less than it leaves out. [basic.fundamental]/2
gives every signed integer type, standard and extended alike, "a corresponding
(but different)" unsigned integer type of the same width, and back again. /3 gives
an integer-class type a width and a signedness, and /5 maps it onto B(I), "a unique
hypothetical extended integer type of the same signedness with the same width" —
which /2 then pairs. So the pairing holds one level down, of the type the subclause
maps an integer-class type onto, and the subclause never lifts it back up to the
integer-class type itself. This library lifts it: `make_signed` and `make_unsigned`
must both name a type, and [meta.trans.sign] says what they name — the type itself
where the signedness already matches, and "the corresponding" type of the other
signedness where it does not.

The lift is this library's, so it is asked for under this library's own name.
`integer_class` is the standard's term, whole and unextended; `integer_like` is
the constraint form of an *exposition-only* concept, which the standard does not
publish and this library does, and it is the one every function here is
constrained on. Adding the pair there rather than to `integer_class` costs the
`_operations` suffix that a third concept needed and keeps a normative term
meaning what it says. The two disagree about exactly one kind of type - an
integer-class type whose counterpart nobody registered - and a case asserts that
disagreement in both directions. The half that is automatic is the matching one,
so a user writes one specialization per type and two per pair. What that buys is a surface with no
seam in it: `unsigned_abs` and `to_chars` used to carry a second constraint for
the counterpart they produce, the two `std::formatter<div_t<I>>` specializations
carried it to stay ordered by subsumption, and the three divisions guarded their
remainder postcondition with an `if constexpr` for the types that could not state
it. All eight sites are gone; `integer_like` says it once, where it was always
being asked.

The cost is a type that has no counterpart and could have divided anyway. An
unsigned integer class with no signed sibling is now refused rather than served,
and the suite has a real one: `unsigned _BitInt(1)`, whose signed counterpart C23
does not allow, C23 setting the signed minimum at two bits. Its registration is
constrained rather than written for every width, so the trait answers no there and
the concept turns the type away, instead of the specialization hard-erroring on a
type the language will not form.

That the failure stays an unsatisfied constraint rather than a hard error is what
the empty `make_signed` and `make_unsigned` primaries are for. Which layer asks
which is what keeps the four traits able to answer at all: `integer_like` asks
them what the counterparts are, so all four are stated over `integer_class`, one
level below it, and nothing asks back up. `is_signed_v` and `is_unsigned_v` are on
that level for a reason of their own as well — a signedness is a property of the
operations, which every integer-class type has, pair or no pair, and an unpaired
one still answers `is_signed_v` as truly as a paired one does.

Below the integer-class branch, both traits ask std for all of what std was
mandated to answer, and not for the part of it this library happens to use.
[meta.trans.sign]/2's domain is "an integral or enumeration type other than cv
`bool`", so the enumerations are in it, and the branch says so rather than
stopping at `std::integral` — the same rule `is_signed_v` and `is_unsigned_v`
already follow by delegating to std for every type std describes. An enumeration
therefore gets an answer here and reaches nothing else: `integer_class` turns it
away at `++a` long before the pairing clause is asked, so no concept admits one
and no function accepts one. The only conjunct left on that branch is cv `bool`,
and it is not there to mirror `integer_like`'s exclusion of it — that one
short-circuits first, so nothing in this library ever asks the traits about
`bool`. It is there because `bool` is integral, so it matches the branch, and std
declines to pair it anyway: libstdc++ declares the specialization without defining
it, libc++ diagnoses it. Deriving from that non-answer is a hard error rather than
an empty trait, and a trait that hard-errors cannot be asked in a
requires-expression — which is precisely how `integer_like` asks for the pair. The
clause answers `false` for `bool` only because this conjunct keeps the failure
soft. `remove_cv_t` is load-bearing one level down for the same reason:
`is_integral_v<bool const>` is `true`, so `same_as<T, bool>` alone would let
`bool const` into the branch and hard-error there.

One corner of that domain is not portable, and this library forwards it rather
than deciding it. An enumeration whose underlying type is `bool` satisfies the
Mandates as written — it is an enumeration type, and it is not cv `bool` — and
libstdc++ answers for it by size, `unsigned char`. libc++ rejects it: "`make_unsigned`
is only compatible with non-bool integers and enum types, but was given ... whose
underlying type is `bool`", looking through the enumeration to a type the Mandates
excludes only when it is `T` itself. Asking std for std's domain means inheriting
that disagreement, which is the same posture the rest of this branch takes, so no
test pins it — a suite that asserted either answer would be asserting a standard
library and not this one.

The three divisions are constrained on `integer_like` alone, and deliberately.
They never form the counterpart to compute with — `/` and `%` are all their bodies
want, and the subclause supplies both. It appears in exactly one place, the
assertion that the remainder is smaller than the denominator, which is written
over magnitudes because `|MIN|` fits in no type but the counterpart:

```cpp
assert(xstd::unsigned_abs(rT) < xstd::unsigned_abs(denom));
```

That assertion used to be reachable only through an `if constexpr`, because a type
admitted without a counterpart could not state it. Now every type that gets this
far has one, so the bound is checked at every width and every signedness, and no
debug-only diagnostic decides what `div` divides. Spelling it `rT / denom == 0`
would avoid the counterpart, but it would check the bound with the very operator
that produced the quotient, and an assertion correlated with what it checks is the
thing the unsigned branches above were relieved of.

The calls these functions make to each other are qualified. Unqualified, ADL adds
the argument's own namespace, where a non-template beats a constrained template
outright — Boost.Int128 has exactly that, a `div` returning its own `i128div_t`
and `u128div_t`, which a structured binding takes apart as happily as xstd's
`div_t`. So `euclidean_div` and `floored_div` had been calling Boost's `div` over
Boost's types, for both signednesses, agreeing on the numbers and skipping every
one of `xstd::div`'s assertions. Returning the result rather than destructuring
it is what turned the substitution into a type error.

`abs` has the signed minimum as a precondition and is total over an unsigned
type, whose `min()` is `0`. `unsigned_abs` returns the unsigned counterpart and
can represent that magnitude at every width. The three divisions require a
nonzero denominator; `MIN / -1` is outside their contract, and is a precondition
only a signed type can reach.

### Conditional `noexcept`

All integer operations are `constexpr`. Their `noexcept` is conditional, on
whether the element type's own operations carry the specifier, because
[iterator.concept.winc] does not ask for `noexcept` anywhere — the word does not
occur in the subclause. Requiring it would narrow the extension point to
integer-class types whose authors wrote it: `absl::uint128` is two `uint64_t`
halves with no way to throw and not one `noexcept` in its header, and was turned
away. Dropping it would go too far the other way, since `std::abs` and `std::div`
*are* `noexcept` as both major implementations ship them. Conditioning keeps
both, and refuses neither. `xstd::to_chars` is unconditional in the other
direction, having no `noexcept` at all, because `std::to_chars` has none either.

The predicate is named for what it can see — a type's declarations, not its
behavior. "Integral" rather than "arithmetic", because `is_arithmetic_like` is
the integer-like half *or* floating point and this answers false for every
floating-point type; "operators", because it ranges over a type's operators
rather than its category. It is public because it *is* those six exception
specifications, and a caller asking whether one throws over their own type is
asking exactly this.

Its operations are every requirement `integer_class` states over `const`
operands, and no more. That boundary is not this library's invention:
`absl::uint128` declares each of them `constexpr` and not one of its mutating
operators, so the `const` half is what an integer-class type in the field treats
as its value surface. It is also the only half these functions can reach, each
taking and returning a value. Within it the coverage is total rather than
itemized — a list drawn from what the bodies contain has to be revised whenever a
body changes, and being over-cautious about an operation a function never
performs costs nothing, where missing one it does perform is a wrong
`noexcept(true)`.

None of this would be here if the language could deduce it. `noexcept(auto)` was
proposed against exactly this shape of problem —
[N3207](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3207.htm)
(2010), still open as
[P3166R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3166r0.html)
(2024) — and C++23 has no such thing. The manual `noexcept(noexcept(e))` reaches
only a single-expression function, which these are not.

A `= default` function needs no stand-in and gets neither specifier: defaulted on
its first declaration it is implicitly `constexpr` if the implicit declaration
would be, and its exception specification is computed. So `div_t`'s equality and
`empty_type`'s default constructor and `<=>` write only `[[nodiscard]]`, the one
of the three with no implicit form. Neither is restated for symmetry, because the
two fail differently: a `constexpr` that cannot hold is refused where a constant
expression needs it, while
[P1286R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1286r2.html)
(C++20) removed the rule that deleted such a defaulted function, so an over-broad
`noexcept` buys a `std::terminate` in place of a diagnostic.

### The 128-bit aliases

They are spelled `int128` and `uint128`, without the `_t` that `<cstdint>`'s
exact-width names carry. C reserves typedef names beginning with `int` or `uint`
and ending in `_t` for future additions to `<stdint.h>`. The reservation does not
reach into a user-defined namespace, but `<xstd/cstdint.hpp>` exists precisely
because the standard header lacks the type, so the day it gains one is the day
the two spellings collide. Boost.Int128 renamed the same way during its review.

Their two associations live in `<xstd/cstdint.hpp>`, beside the aliases and not
inside the traits, which is where `test/include/xstd/test/` puts Abseil's and
Boost.Int128's: the header that introduces a pair registers it, so whoever can
name the type has the specializations in scope and cannot reach the type through
a translation unit that would answer differently. It also leaves `make_signed`
and `make_unsigned` knowing about no type in particular. What the aliases still
cannot do is demonstrate the widening on their own, being specialized for by a
header of this library's; the exact-width test lists therefore also carry
Boost.Int128 and `absl::int128`, which no shipped header names, and the two
associations written for each in `test/include/xstd/test/` are the whole of what
a user has to write. Both dependencies are optional; see
[CONTRIBUTING.md](../CONTRIBUTING.md).

The two are not interchangeable, which is why both are here. Boost annotates its
operations and Abseil annotates none, so they land on opposite sides of
`nothrow_integer_operators`. They differ once more: `absl::int128`'s `operator/`
and `operator%` are `constexpr` only where a 128-bit intrinsic backs them, and
out-of-line otherwise. That is the one place these tests assert something about
the target rather than the type. The whole-surface battery in
`test/src/cstdlib.cpp` runs over it everywhere, constant-evaluated where the
intrinsic is there and checked at run time where it is not; its place in the
exact-width lists is conditional on `ABSL_HAVE_INTRINSIC_INT128` instead, those
cases constant-evaluating everything. Only the three divisions object at all, and
those are exactly what the battery covers.

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_specialization_of` and `specialization_of` recognize specializations of
  class templates whose parameters are types.
- `is_signed` and `is_unsigned` are open counterparts of the standard traits.
- `empty_type` and `conditional_data_member_t` support optional
  `[[no_unique_address]]` storage.
- `to_underlying` forwards a plain enum and preserves one wrapped in
  `std::integral_constant`.
- `nothrow_integer_operators` answers whether the conditional `noexcept` holds.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. `nothrow_integer_operators` is
the one concept with no trait beside it, having no standard trait to mirror, and
is spelled the way a caller writes it: inside a `noexcept`.

Which decides where the `is` goes, and the subclause is no guide there. Its
concept is *is-integer-like* — the only concept in the library or the standard
whose name starts with `is`, because it is exposition-only and answers to nobody.
The convention outside it is the opposite and is worth following: `is` marks a
trait, as in `std::is_integral`, and a concept goes bare, as in `std::integral`.
So the concept here is `integer_like`, bare, rather than an `is_integer_like`
transliterating a hyphenated name that was never meant to be public; where a trait
does stand beside a concept, as `is_signed` does beside `signed_integer_like`, the
`is` is what marks which is which.

### `to_underlying`

The [original 2016 sketch](ideas.md#1-convenient-underlying-types-for-scoped-enums)
motivated `to_underlying` with scoped enums used as named tuple and array
indices. Rein Halbersma developed the idea and initial usage evidence with
Walter E. Brown; JeanHeyd Meneide then authored
[P1682R1](https://wg21.link/p1682r1) and carried `std::to_underlying` through
WG21 for C++23. P1682's acknowledgements record those roles. The xstd overload
complements it: given an enum value wrapped in `std::integral_constant`, it
returns an `integral_constant` of the underlying type, preserving the value at
the type level.

A second overload forwards a plain enum to `std::to_underlying`, so `xstd::` is
one spelling over both forms rather than a name a caller has to remember to
switch away from for the unwrapped case. It is an overload rather than a
using-declaration because the constraint is then written where it applies, as
the wrapped one writes its own.

### Character conversion

`xstd::to_chars` widens `std::to_chars` to every integer-like type. Where the
standard library covers the type it *is* that call, so callers get the tuned
implementation; where it does not, the digits are produced here to the same
specification, bases 2 through 36 and `value_too_large` included. The default
base is written as the literal `10`, as [charconv.to.chars] writes it, so the two
signatures read alike; `readability-magic-numbers` is suppressed a line at a time
rather than taught to ignore `10` throughout.

The digits come off the unsigned counterpart of `I`, the only type in which the
magnitude of `min()` is representable, and everything after that is unsigned
arithmetic. Staying in `I` would work, `%` yielding a remainder of the value's
own sign that a digit table read backwards absorbs, but it pays for the sign at
every digit rather than once and forces both loops to test `rest / radix == 0`
where an unsigned magnitude tests `rest >= radix` — one division per digit rather
than two, and for an integer-class type a division is a call. libstdc++ makes the
same reduction in `__to_chars_i`, and the loop shape here is its
`__detail::__to_chars`.

Neither loop counts characters. The first walks a pointer forward from where the
most significant digit goes, and a step onto `last` is the short buffer; the
second walks it back down, writing as it goes. The buffer's end is the walk's own
bound rather than a count, which keeps the comparison between two pointers and
leaves no signed-to-unsigned mismatch to answer; the one place a length is asked
for at all is the check ahead of the walk, which spells it `std::distance`. The
sign is written last, the walk stepping back into the one position it reserved
and no digit claimed; writing it first is what obliges libstdc++ to leave a `'-'`
in a buffer it then reports as too small. That the walk cannot start before there is
room for the first digit and the sign is why `value_too_large` has two returns,
both covered per type since gcov records them per instantiation.

The two overloads are kept apart by subsumption rather than by hand: the
delegating one requires a `std::to_chars` call to be well-formed *on top of*
`integer_like`, and a conjunction subsumes its left operand, so it wins partial
ordering wherever both are viable. Spelling the other as the negation would work
too, but a negated atomic constraint does not subsume, so exclusivity and
exhaustiveness would become an invariant to maintain across two edits instead of
a property of the constraints. The constraint is that call itself, there being no
standard concept for "the standard library converts this type"; it stays an answer
rather than an error because `I` is the overload's own parameter, which keeps the
expression dependent until the constraint is checked. It is spelled as a named
concept a few lines above rather than inline, so that each overload's
requires-clause is one line and one conjunction — a requires-expression nested
inside a requires-clause reads as neither one thing nor two, and with
`ColumnLimit: 0` its continuation indent is the formatter's to choose.

Their order in the header is reading order: the standard's own call, then the
deletion the standard also makes, then the fallback for what neither covers. It
once could not be. gcov names only the first group of functions sharing a start
line in a file, and gcovr keyed its cross-translation-unit merge on those names,
so an unnamed group merged with nothing; the digits body contains lines no other
translation unit can reach — the short-buffer returns — so it had to be the named
one, and ordered the other way the file measured 98% and the coverage gate
failed. The same sensitivity was why the digits body kept its sign and magnitude
as two plain declarations. Passing `--merge-lines` to gcovr merges a template's
instantiations by line rather than by name, which dissolved that constraint and
with it the reason the digits overload came first.

Two places the standard library does not cover are worth separating. Integer-class
types are never covered, `to_chars` being specified over the built-in integer
types. But the *built-in* 128-bit types are not covered either in this dialect:
libstdc++ gates its 128-bit pair on `__GLIBCXX_TYPE_INT_N_0`, predefined only
outside `__STRICT_ANSI__`, so with `CMAKE_CXX_EXTENSIONS OFF` the overload is
absent and the call is *ambiguous*. Detection therefore has to survive an
ambiguity rather than a clean deduction failure, which it does: overload
resolution failing is still failing in the immediate context.

`<format>` goes the other way for the same underlying reason, libstdc++ adding a
`formatter<__int128, CharT>` *only* under `__STRICT_ANSI__`. So the three
facilities disagree about one type along different axes: `<format>` works only in
the strict dialect, `<charconv>` only outside it, and `<ostream>` in neither.
`bool` is integer-like, and `to_chars` is deleted for it as in the standard.

### Formatting

`<xstd/format.hpp>` formats a `div_t` as `(quot, rem)`, through two partial
specializations of `std::formatter`. `div_t` is tuple-like, so where the standard
library can format a tuple of the element type the specialization to use inherits
`std::formatter<std::tuple<I const&, I const&>>` and hands it `std::tie` — the
standard's own rendering, with no intermediate string.

That delegation can be missing in two ways that do not coincide: the element type
may not be formattable, which is the Microsoft STL's 128-bit classes, or tuple
formatting itself may be missing, p2286 having reached libstdc++ only in GCC 15.
Testing `formattable` on the *tuple* covers both with one predicate. Where it
does not hold, the other specialization renders the members through
`xstd::to_chars` into an inherited string formatter, so `div_t` formats on every
implementation and for either signedness — for every element type `xstd::to_chars`
itself covers, which is to say every one with an unsigned counterpart.

The choice is left to partial ordering, on the same footing as `to_chars`'s
overloads: the tuple one requires the base it inherits, spelled where it applies,
on top of the counterpart the other asks for, so its constraints are a superset
and it wins wherever both are viable. Both produce `(quot, rem)`, so which runs is
not observable in the output — only in the spec grammar, `parse()` being
inherited, which is why the README says so.

xstd specializes `std::formatter` only for `div_t`, which is program-defined. The
128-bit types are not xstd's to specialize for: they are built-ins or
standard-library types, which [namespace.std]/2 does not cover. There is no
stream inserter either — a built-in has no associated namespace for ADL, and a
standard-library type's is one no program may add to. Boost.Test, the only
consumer that needed it, asks through `print_log_value`, which the tests
specialize directly.

## Integer division across languages

The names are not consistent across programming languages. The table compares
integer operations with xstd's three conventions. An entry names a matching
quotient/remainder pair unless it says **quotient only** or **remainder only**.
It is a guide to recognizable spellings, not a promise about every language
version or numeric type. Rows with similar sets of conventions are kept
together. The language survey in [P3724R4](https://wg21.link/p3724r4) supplies
additional points of comparison.

| Language | Truncated integer division | Euclidean integer division | Floored integer division |
| :------- | :------------------------- | :------------------------- | :----------------------- |
| C (C99 and later) | `/`, `%` | — | — |
| C++ (C++11 and later) | `/`, `%` | — | — |
| Objective-C | `/`, `%` | — | — |
| C# | `/`, `%` | — | — |
| D | `/`, `%` | — | — |
| F# | `/`, `%` | — | — |
| Go | `/`, `%` | — | — |
| JavaScript `BigInt` | `/`, `%` | — | — |
| PHP | `intdiv`, `%` | — | — |
| Scala | `/`, `%` | — | — |
| Swift | `/`, `%` | — | — |
| Erlang | `div`, `rem` | — | — |
| OCaml | `/`, `mod` | — | — |
| GLSL | `/` (**quotient only**) | — | — |
| Rust | `/`, `%` | `div_euclid`, `rem_euclid` | — |
| Dart | `~/`, `remainder` | `%` (**remainder only**) | — |
| Java | `/`, `%` | — | `Math.floorDiv`, `Math.floorMod` |
| Kotlin | `/`, `%` | — | `floorDiv`, `mod` |
| Zig | `@divTrunc`, `@rem` | — | `@divFloor`, `@mod` |
| Haskell | `quot`, `rem`, `quotRem` | — | `div`, `mod`, `divMod` |
| Julia | `div`, `rem` | — | `fld`, `mod` |
| Common Lisp | `truncate`, `rem` | — | `floor`, `mod` |
| Prolog | `//`, `rem` | — | `div`, `mod` |
| Scheme | R7RS `truncate-quotient`, `truncate-remainder` | R6RS `div`, `mod`, `div-and-mod` | R7RS `floor-quotient`, `floor-remainder` |
| Ada | `/`, `rem` | — | `mod` (**remainder only**) |
| Clojure | `quot`, `rem` | — | `mod` (**remainder only**) |
| Fortran | `/`, `MOD` | — | `MODULO` (**remainder only**) |
| Ruby | `remainder` (**remainder only**) | — | `div`, `%`, `modulo`, `divmod` |
| Python | — | — | `//`, `%`, `divmod` |
| Lua | — | — | `//`, `%` |
| R | — | — | `%/%`, `%%` |
| Standard ML | — | — | `div`, `mod` |
| Perl | — | — | `%` (**remainder only**) |
| CSS | `rem()` (**remainder only**) | — | `mod()` (**remainder only**) |

The paired operations all satisfy `numer == denom * quot + rem`, but choose a
different remainder for negative inputs:

- **Truncated (`quot`/`rem`)** rounds the quotient toward zero; a nonzero
  remainder has the numerator's sign.
- **Euclidean** chooses `0 <= rem < |denom|`, so the remainder is always
  nonnegative.
- **Floored (`div`/`mod`)** rounds the quotient toward negative infinity; a
  nonzero remainder has the denominator's sign.

No single pair of integer operands can make all three results different. For a
positive numerator and negative denominator, truncated and Euclidean division
agree. For a negative numerator and positive denominator, Euclidean and
floored division agree. When both are negative, truncated and floored division
agree; when both are positive, all three agree.

The terminology follows
[Division and Modulus for Computer Scientists](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/divmodnote-letter.pdf).
An operation limited to a positive modulus does not establish the complete
Euclidean convention. This is why older annotations naming Maple and Pascal
are omitted, and why traditional Scheme `modulo` is not placed in the
Euclidean column.

## Requirements and evolution

xstd requires a conforming C++23 compiler, and the CMake project CMake 3.28 or
later, exporting the header-only `xstd::xstd` target. Consumers build neither the
tests nor their dependencies. New facilities should stay small, portable and
motivated by current practice; one that enters the standard library can be
retired, and the baseline can advance to C++26 once that is a practical default.

## CI policy

The matrix follows the channel model used by
[apt.llvm.org](https://apt.llvm.org/): **stable** is the established release,
**qualification** the newest release still being qualified, and **development**
the current development branch. These are tested across the three main compiler
families, the three main standard libraries and the three main desktop platforms,
with MinGW, Apple Clang and Clang-CL filling out the remaining combinations.
Development jobs are required where a usable development compiler exists; the
README records the versions currently assigned to each channel. Building the
tests requires the dependencies documented in
[CONTRIBUTING.md](../CONTRIBUTING.md).
