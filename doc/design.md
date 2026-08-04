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

The arithmetic functions use one constrained template per operation. This
covers every signed integer-like width without families such as `abs`, `labs`,
`llabs`, and `imaxabs`. A function returns the argument type rather than a
promoted type, and a two-argument function requires both arguments to have the
same type unless the caller explicitly selects one.

`abs` has the usual signed-minimum precondition. `uabs` returns the unsigned
counterpart and can represent that magnitude. `div`, `euclidean_div`, and
`floored_div` require a nonzero denominator, and `MIN / -1` remains outside
their contract. All integer operations are `constexpr` and `noexcept`.

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

### Formatting

`div_t` is tuple-like. `<xstd/format.hpp>` provides its formatter when the
element type and standard library support tuple formatting. The separate
`<xstd/ostream.hpp>` overload exists mainly for test diagnostics. Formatting is
the only public area with an additional standard-library feature requirement;
the rest of xstd does not depend on `std::format`.

## Integer division across languages

The names are not consistent across programming languages. The table compares
integer operations with xstd's three conventions. An entry names a matching
quotient/remainder pair unless it says **remainder only**. It is a guide to
recognizable spellings, not a promise about every language version or numeric
type. Rows with similar sets of conventions are kept together.

| Language | Truncated integer division | Euclidean integer division | Floored integer division |
| :------- | :------------------------- | :------------------------- | :----------------------- |
| C (C99 and later) | `/`, `%` | — | — |
| C++ (C++11 and later) | `/`, `%` | — | — |
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
| R | — | — | `%/%`, `%%` |
| Standard ML | — | — | `div`, `mod` |
| Perl | — | — | `%` (**remainder only**) |

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
agree; when both are positive, all three agree. For example:

```cpp
#include <xstd/cstdlib.hpp>

constexpr int numer = -8;
constexpr int denom = -3;

constexpr auto truncated = xstd::div(numer, denom);           // (2, -2)
constexpr auto euclidean = xstd::euclidean_div(numer, denom); // (3,  1)
constexpr auto floored = xstd::floored_div(numer, denom);     // (2, -2)

constexpr auto common_invariants = [](auto result) {
    return numer == denom * result.quot + result.rem
        && xstd::uabs(result.rem) < xstd::uabs(denom);
};

static_assert(common_invariants(truncated));
static_assert(common_invariants(euclidean));
static_assert(common_invariants(floored));

static_assert(xstd::sign(truncated.rem) == xstd::sign(numer));
static_assert(euclidean.rem >= 0);
static_assert(xstd::sign(floored.rem) == xstd::sign(denom));
static_assert(truncated == floored); // unavoidable for this sign pattern
```

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
STL), and the three main desktop platforms (Linux, macOS, and Windows). MinGW,
Clang with both standard libraries, Apple Clang, and Clang-CL fill out the
relevant compiler/library/platform combinations. Development entries are
required when an upstream project publishes a usable development toolchain;
the README records the exact versions currently assigned to each channel.

The library itself has no external dependency. Building its tests requires the
development dependencies documented in [CONTRIBUTING.md](../CONTRIBUTING.md).
