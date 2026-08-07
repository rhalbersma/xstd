# Design notes

## Purpose

xstd is a header-only collection of small C++ standard-library extensions.
There is no single proposal or grand design behind it: the library collects
useful facilities suggested by existing practice when they can be implemented
portably and with a standard-library-style interface.

The current baseline is C++23. A future release may move to C++26 once support
is commonplace across the tested compilers and standard libraries. Consumers
need no third-party dependencies; dependencies used by the test suite are a
development concern only.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents
  it. This applies to the integer functions, `aligned_size`, and
  `to_underlying`.
- **Generalize integer support.** The standard integral concepts and traits are
  closed over built-in types. xstd's `_like` concepts and traits also accept
  integer-class types, including the platform 128-bit types, by checking their
  behavior and `std::numeric_limits` specialization.
- **Keep metaprogramming small.** Facilities such as `specialization_of`,
  `is_specialization_of`, `empty_type`, and `conditional_data_member_t` solve
  common local problems without introducing a framework.
- **Make semantics explicit.** The three division functions name their rounding
  convention rather than hiding it behind `/` and `%` behavior.
- **Stay modular and dependency-free.** Public facilities live in focused
  headers and the `xstd` namespace. Linking `xstd::xstd` adds include paths and
  the C++23 requirement, but no runtime library or transitive package.

## API shape

### Integer-like types

`integral_like`, `signed_integral_like`, and `unsigned_integral_like` widen the
corresponding standard concepts to structurally recognized integer-class types.
The related `_like` traits follow the same rule. Built-in integers work without
customization; a user-defined signed/unsigned pair supplies the opposite
`make_signed_like` and `make_unsigned_like` specializations.

A template parameter is named for the concept constraining it: `I` under
`integral_like`, `S` under `signed_integral_like`. The letters carry the
constraint into the body, where `S{-1}` reads as something the type can hold
and `I{-1}` would not. Where a parameter is deduced from a type that is already
constrained - `S` from `div_t<S>` in the formatter - the constraint is spelled
anyway: it can never be the reason a specialization fails to match, but the
alternative reads as though `div_t` were open to any element type.

The arithmetic functions use one constrained template per operation. This
covers every signed integer-like width without families such as `abs`, `labs`,
`llabs`, and `imaxabs`. A function returns the argument type rather than a
promoted type, and a two-argument function requires both arguments to have the
same type unless the caller explicitly selects one.

`abs` has the usual signed-minimum precondition. `unsigned_abs` returns the unsigned
counterpart and can represent that magnitude. `div`, `euclidean_div`, and
`floored_div` require a nonzero denominator, and `MIN / -1` remains outside
their contract. All integer operations are `constexpr` and `noexcept`.

The 128-bit aliases are spelled `int128` and `uint128`, without the `_t` that
`<cstdint>`'s exact-width names carry. C reserves typedef names beginning with
`int` or `uint` and ending in `_t` for future additions to `<stdint.h>`, and
`int128_t` is exactly that pattern. The reservation does not reach into a
user-defined namespace, so `xstd::int128_t` would have been legal; the point is
that `<xstd/cstdint.hpp>` exists precisely because the standard header lacks the
type, so the day it gains one is the day the two spellings collide for anyone
who has both namespaces in scope. Boost.Int128 renamed the same way, for the
same reason, during its review.

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_specialization_of` and `specialization_of` recognize specializations of
  class templates whose parameters are types.
- `is_integral_like`, `is_arithmetic_like`, `is_signed_like`, and
  `is_unsigned_like` are open counterparts of the standard traits.
- `empty_type` and `conditional_data_member_t` support optional
  `[[no_unique_address]]` storage.
- `to_underlying` preserves an enum wrapped in `std::integral_constant`.

Concept spellings are provided when the standard library has an analogous
concept; otherwise the trait is the interface. Detailed constraints belong in
the headers and tests rather than being duplicated here.

### `to_underlying`

The [original 2016 sketch](ideas.md#1-convenient-underlying-types-for-scoped-enums)
motivated `to_underlying` with scoped enums used as named tuple and array
indices. Rein Halbersma developed the idea and initial usage evidence with
Walter E. Brown; JeanHeyd Meneide then authored
[P1682R1](https://wg21.link/p1682r1) and carried `std::to_underlying` through
WG21 for C++23. P1682's acknowledgements record those roles.

The xstd overload complements that standard function rather than replacing it:
given an enum value wrapped in `std::integral_constant`, it returns an
`integral_constant` of the underlying type and preserves the value at the type
level.

### Character conversion

`xstd::to_chars` widens `std::to_chars` to every integral-like type. Where the
standard library already covers the type it *is* that call, so callers get the
tuned implementation; where it does not, the digits are produced here to the
same specification, including bases 2 through 36 and `value_too_large` on a
short buffer.

The two are overloads on one name rather than one function branching on an `if
constexpr`, and they are kept apart by subsumption rather than by hand: the
delegating overload requires a `std::to_chars` call to be well-formed *on top
of* `integral_like`, and a conjunction subsumes its left operand, so wherever
both are viable the delegating one is more constrained and wins partial
ordering. Spelling the other overload's constraint as the negation would work
too, but a negated atomic constraint does not subsume, so exclusivity and
exhaustiveness would then be an invariant someone has to maintain across two
edits instead of a property of the constraints.

The constraint is that call, written where it applies, rather than a named
predicate of xstd's own standing in for it. There is no standard concept for
"the standard library converts this type", so the requires-expression names
`std::to_chars` directly; it stays an answer rather than an error because `T`
is the overload's own template parameter, which keeps the expression dependent
until the constraint is checked. Asking the same question about a *concrete*
type, as the tests do, still needs a named concept, and the test defines one.

Their order in the header is not editorial. gcov names only the first group of
functions sharing a start line in a file, and gcovr keys its merge across
translation units on those names, so an unnamed group merges with nothing. The
digits body contains a line no other translation unit can reach - the
short-buffer return - so it has to be the named one; every line of the
delegating body runs wherever it is instantiated at all, so leaving that group
unnamed costs nothing. Ordered the other way the file measures 98% and the
coverage gate fails.

The tests check the two agree byte for byte at every base, because a divergence
would surface only on the platforms taking the other overload. Since the
overloads are selected by constraint, the digits one cannot be named for a type
the standard library covers - so the comparison goes through a type it does
not, and `xstd::int128` holds every value of a narrower type.

Two places the standard library does not cover are worth naming, because they
are not the same place. Integer-class types are never covered: `to_chars` is
specified over the built-in integer types. But the *built-in* 128-bit types are
not covered either, in the dialect this library compiles in. libstdc++ generates
its `to_chars` overload set per built-in width and gates the 128-bit pair on
`__GLIBCXX_TYPE_INT_N_0`, which the compiler predefines only outside
`__STRICT_ANSI__`; with `CMAKE_CXX_EXTENSIONS OFF` the overload is absent and
the call is *ambiguous*, since the argument converts equally well to every
narrower width. Detection therefore has to survive an ambiguity rather than a
clean deduction failure, which it does: overload resolution failing is still
failing in the immediate context, so the constraint is unsatisfied rather than
ill-formed.

`<format>` goes the other way, and for the same underlying reason. libstdc++
adds a `formatter<__int128, CharT>` specialization *only* under
`__STRICT_ANSI__` - where the type stops being integral and the generic path
would miss it - and its integer formatter shadows `to_chars` with a shim onto
the internal conversion template, because the public overload set is the part
that is missing. So the three facilities disagree about the same type along
different axes: `<format>` works only in the strict dialect, `<charconv>` only
outside it, and `<ostream>` in neither. Only code asking for nothing beyond
`%`, `/`, and comparison behaves the same everywhere.

`bool` is integral-like, and `to_chars` is deleted for it, as in the standard.

### Formatting

`<xstd/format.hpp>` formats a `div_t` as `(quot, rem)`, through two partial
specializations of `std::formatter` - one per way of producing that. `div_t` is
tuple-like, so where the standard library can format a tuple of the element
type the specialization to use is the one inheriting
`std::formatter<std::tuple<S const&, S const&>>` and handing it `std::tie` -
the standard's own rendering, with no intermediate string.

That delegation is not always available, and the two ways it can be missing do
not coincide. The element type may not be formattable, which is the Microsoft
STL's 128-bit classes. Or tuple formatting itself may be missing: p2286 reached
libstdc++ in GCC 15, so before that `std::formattable<std::tuple<int const&,
int const&>>` is false even though `int` is perfectly formattable. Testing
`formattable` on the *tuple* rather than on the element covers both with one
predicate. Where it does not hold, the other specialization renders the members
through `xstd::to_chars` into an inherited string formatter, which asks nothing
beyond what `signed_integral_like` already guarantees - so `div_t` formats for
every type it accepts, on every implementation.

The choice between them is left to partial ordering, on the same footing as
`to_chars`'s two overloads: the tuple one requires
`std::formattable<std::tuple<S const&, S const&>, CharT>` - the base it
inherits, spelled out where it applies rather than behind a predicate of xstd's
own - and the other requires nothing, so they are equally specialized on their
arguments and the more constrained one wins wherever it matches at all. That is
what replaces a `std::conditional_t` base and an `if constexpr` in `format()` -
each specialization now names its own base outright and is written against that
base alone.

Both spellings produce `(quot, rem)`, so which one runs is not observable in
the output. It is observable in the spec grammar, since `parse()` is inherited:
the tuple base accepts the tuple specs, the string base accepts precision, and
fill, alignment and width are common to both.

xstd specializes `std::formatter` only for `div_t`, which is program-defined.
The 128-bit types are not xstd's to specialize for: they are built-ins or
standard-library types, which [namespace.std]/2 does not cover, and where the
standard library already provides a formatter - libstdc++ does for `__int128`,
precisely under `__STRICT_ANSI__` - a specialization would displace one that
handles the whole spec grammar.

There is no stream inserter. For the 128-bit types xstd could not provide one
that is reachable: a built-in has no associated namespace for ADL to find, and
a standard-library type's associated namespace is one no program may add to.
Boost.Test, the only consumer that needed it, asks for printing through
`print_log_value`, which the tests specialize directly. Formatting is the only
public area with an additional standard-library requirement; nothing else in
xstd depends on `<format>`.

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

xstd requires a conforming C++23 compiler. The CMake project requires CMake
3.28 or later and exports the header-only `xstd::xstd` target. Consumers build
neither the tests nor their dependencies when xstd is added with
`FetchContent` or `add_subdirectory`.

New facilities should remain small, portable, and motivated by current
practice. They do not need to fit a larger roadmap. A facility that enters the
standard library can be retired, while the language baseline can advance to
C++26 after mainstream toolchains make that a practical default.

## CI policy

The CI matrix follows the channel model used by
[apt.llvm.org](https://apt.llvm.org/):

- **Stable** is the established release.
- **Qualification** is the newest release still being qualified.
- **Development** tracks the compiler or library's current development branch.

These channels are tested across the three main compiler families (GCC, Clang,
and MSVC), the three main standard libraries (libstdc++, libc++, and the MSVC
STL), and the three main desktop platforms (Linux, macOS, and Windows). MinGW
with libstdc++, Clang with both standard libraries, Apple Clang, and Clang-CL
fill out the relevant compiler/library/platform combinations. Development CI
jobs are required when a usable development compiler is available;
the README records the exact versions currently assigned to each channel.

The library itself has no external dependency. Building its tests requires the
development dependencies documented in [CONTRIBUTING.md](../CONTRIBUTING.md).
