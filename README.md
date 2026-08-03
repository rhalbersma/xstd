# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml)
[![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml)
[![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml)
[![Clang-libc++](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml)
[![Apple Clang](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/rhalbersma/xstd/badge)](https://securityscorecards.dev/viewer/?uri=github.com/rhalbersma/xstd)

xstd is a header-only C++23 library for small standard-library extensions that can be implemented portably with stable compiler technology. It aims to prototype future-stdlib-style facilities without requiring experimental language features. See [doc/design.md](doc/design.md) for the design philosophy tying the headers together.

| Header | Additions, in order of definition | Description | Reference |
| :----- | :-------------------------------- | :---------- | :-------- |
| `<xstd/concepts.hpp>` | `integral_like` <br> `signed_integral_like` <br> `unsigned_integral_like` <br> `specialization_of` | Constraint form of `is_integral_like` <br> Open form of `std::signed_integral` <br> Open form of `std::unsigned_integral` <br> Constraint form of `is_specialization_of` | [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) |
| `<xstd/cstdint.hpp>` | `int128_t` <br> `uint128_t` | Platform 128-bit signed integer <br> Platform 128-bit unsigned integer | none <br> none |
| `<xstd/cstdlib.hpp>` | `div_t` <br> `sign` <br> `abs` <br> `uabs` <br> `div` <br> `euclidean_div` <br> `floored_div` | Defaulted equality comparison, `std::format` support where the element type has it <br> `constexpr`, any signed integer-like type <br> `constexpr`, any signed integer-like type <br> Total `\|x\|`, returning the unsigned counterpart <br> `constexpr`, any signed integer-like type <br> Euclidean division <br> Floored division | [p2286r8](https://wg21.link/p2286r8), [p3391](https://wg21.link/p3391) (C++29, not yet implemented) <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/concepts/exposition_only.hpp>` | `integer_class_type` | Structural form of an integer-class type | [iterator.concept.winc] |
| `<xstd/memory.hpp>` | `aligned_size` | Round a size up to a power-of-two alignment | none |
| `<xstd/type_traits.hpp>` | `empty_type` <br> `is_integral_like` <br> `is_arithmetic_like` <br> `is_signed_like` <br> `is_unsigned_like` <br> `is_specialization_of` <br> `make_signed_like` <br> `make_unsigned_like` <br> `conditional_data_member_t` | A tagged empty type <br> `std::is_integral`, opened to integer-class types <br> `std::is_arithmetic`, opened through the integral half <br> `std::is_signed`, opened the same way <br> `std::is_unsigned`, opened the same way <br> Is a type a class template specialization? <br> Open, user-specializable `std::make_signed` <br> Open, user-specializable `std::make_unsigned` <br> A conditionally present member | none <br> [iterator.concept.winc] (`is-integer-like`) <br> none <br> none <br> none <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>` | `to_underlying` | `std::integral_constant` overload | none |

## Using xstd

xstd is consumed through [CMake](https://cmake.org/), which is the build system it supports: it exports an `xstd::xstd` interface target, ships a package config for `find_package`, and needs CMake 3.28 or later. All three integration paths below hand you that same target.

xstd isn't published to a package registry, so the default way to add it is `FetchContent`, which needs no separate install step:

```cmake
include(FetchContent)
FetchContent_Declare(
    xstd
    GIT_REPOSITORY https://github.com/rhalbersma/xstd.git
    GIT_TAG main # or a release tag
)
FetchContent_MakeAvailable(xstd)
target_link_libraries(my_target PRIVATE xstd::xstd)
```

If you've already installed xstd yourself (e.g. `cmake --install`, or your own package manager integration), use `find_package` instead:

```cmake
find_package(xstd 0.1.0 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE xstd::xstd)
```

If you vendor the source directly (e.g. a git submodule), use `add_subdirectory` (xstd's own tests, and the dependencies they need, are only built when xstd is the top-level project, so nothing needs to be disabled):

```cmake
add_subdirectory(external/xstd)
target_link_libraries(my_target PRIVATE xstd::xstd)
```

The target publishes the public headers and requires C++23. You can also include individual headers directly, such as `<xstd/cstdlib.hpp>` or `<xstd/type_traits.hpp>`.

`<xstd/cstdint.hpp>` provides `xstd::int128_t` and `xstd::uint128_t`. They alias
the compiler's `__int128` types on GCC and Clang, and the Microsoft STL's
`std::_Signed128` and `std::_Unsigned128` integer classes on MSVC.

None of this needs xstd's own test suite, which only matters when working on the library itself; see [CONTRIBUTING.md](CONTRIBUTING.md) for how to build and run it.

## Examples

### Compile-time enum conversion

`xstd::to_underlying` complements `std::to_underlying` by preserving an enum value that is wrapped in `std::integral_constant`:

```cpp
#include <type_traits>
#include <xstd/utility.hpp>

enum class color : unsigned { red = 1 };

using red = std::integral_constant<color, color::red>;
using value = decltype(xstd::to_underlying(red{}));

static_assert(value::value == 1);
static_assert(std::is_same_v<value, std::integral_constant<unsigned, 1>>);
```

### Rounding a size up to an alignment

`xstd::aligned_size` rounds a requested size up to the next multiple of an alignment, e.g. to turn a bit count into a whole number of storage blocks. Its alignment must be a power of two and the rounded result must fit in `std::size_t`; violations are guarded by `assert`:

```cpp
#include <xstd/memory.hpp>

static_assert(xstd::aligned_size(64, 100) == 128);
```

### Absolute value without a precondition

`xstd::abs` returns the signed type, like `<cstdlib>`'s, so the most negative value is outside its contract. `xstd::uabs` returns the unsigned type, which can represent that magnitude, and so has no precondition:

```cpp
#include <climits>
#include <cstdint>
#include <xstd/cstdlib.hpp>

static_assert(xstd::abs(-2) == 2);
static_assert(xstd::uabs(-2) == 2u);

// |INT_MIN| is one past INT_MAX, so only the unsigned form can return it
static_assert(xstd::uabs(INT_MIN) == static_cast<unsigned>(INT_MAX) + 1u);

// one template per operation, so every signed integer-like type is covered,
// including the two widths the labs/llabs/imaxabs naming has no name for
static_assert(xstd::uabs(std::int8_t{-128}) == std::uint8_t{128});
```

### Division helpers

`xstd::euclidean_div` and `xstd::floored_div` make the desired division convention explicit for negative inputs:

```cpp
#include <format>
#include <xstd/cstdlib.hpp>

constexpr auto euclidean = xstd::euclidean_div(-8, 3);
static_assert(euclidean.quot == -3);
static_assert(euclidean.rem == 1);

constexpr auto floored = xstd::floored_div(-8, 3);
static_assert(floored.quot == -3);
static_assert(floored.rem == 1);

auto const text = std::format("{}", floored); // "(-3, 1)"
```

`xstd::div`, `xstd::euclidean_div`, and `xstd::floored_div` require a nonzero denominator. Like built-in signed integer division, `MIN / -1` is outside their contract. `xstd::div` follows C++'s truncated division semantics, `xstd::euclidean_div` always returns a nonnegative remainder, and `xstd::floored_div` returns a remainder with the divisor's sign unless the remainder is zero. Each returns `xstd::div_t<T>` for its argument type `T`; class template argument deduction means the result can still be written `xstd::div_t{quot, rem}`.

Formatting `xstd::div_t` requires C++23 standard-library support for formatting tuple-like values. This is covered by the continuously tested compiler and standard-library versions below. Prefer `std::format`/`std::print` over its `operator<<`, which exists only for test-framework diagnostics.

Formatting is the one operation here that isn't `constexpr`: the tuple formatter it delegates to isn't either. That is tracked rather than fixed in place — once a standard library implements [p3391](https://wg21.link/p3391) (`constexpr std::format`, approved for C++29) and defines `__cpp_lib_constexpr_format`, `std::format("{}", xstd::floored_div(-8, 3))` becomes usable in a constant expression with no change on the calling side. `operator<<` stays runtime-only whatever happens, since `std::ostream` does.

### Width and constraints

Every function in `<xstd/cstdlib.hpp>` is a single function template constrained to `xstd::signed_integral_like`, rather than the four fixed-width overloads (`abs`/`labs`/`llabs`/`imaxabs` and friends) `<cstdlib>` and `<cinttypes>` declare. Three consequences at the call site:

- **Every signed integer-like type is covered**, including `std::int8_t` and `std::int16_t`, which the `<cstdlib>` naming has no name for, and including the 128-bit types. `std::signed_integral` would not have reached those: it is spelled over `std::is_integral`, which excludes `__int128` on libstdc++ in the strictly conforming dialect this library targets, and excludes every *class* type unconditionally — which is what a 128-bit integer is where there is no built-in one (libstdc++'s `__max_diff_type`, the MSVC STL's `std::_Signed128`).
- **The result type is the argument type, not the promoted type.** `xstd::abs` of an `std::int16_t` is an `std::int16_t`, and its precondition is `std::int16_t`'s — the most negative value of the *argument* type is outside its contract (guarded by an `assert`), just like `MIN / -1` is for the division helpers. Callers who want the old promoting behavior write `xstd::abs(+x)` or `xstd::abs<int>(x)`.
- **Two-argument templates deduce one `T` from both arguments**, so a mixed-width call like `xstd::div(8, 3L)` is a deduction failure rather than a silent conversion. Spell the intent as `xstd::div<long>(8, 3L)`.

`xstd::sign` isn't part of `<cstdlib>`, and always returns a plain `int` whatever its argument's width.

All six are unconditionally `noexcept`. Integer-class types must expose every required construction, comparison, arithmetic, bitwise, shift, compound-assignment, and increment/decrement expression as syntactically `noexcept`.

A built-in type needs nothing from you. For an integer-class pair, each type is automatically its own counterpart on the side it already occupies; the two cross-directions are written as one-line metafunction-forwarding specializations:

```cpp
template<>
struct xstd::make_signed_like<my_uint128> : std::type_identity<my_int128> {};

template<>
struct xstd::make_unsigned_like<my_int128> : std::type_identity<my_uint128> {};

static_assert(xstd::signed_integral_like<my_int128>);
```

Formatting is the one thing that does not follow: `std::formatter<xstd::div_t<T>>` delegates to the tuple formatter, so a `div_t` over a type without a `std::formatter` simply isn't formattable. The specialization's body is instantiated only where it is used, so this costs nothing until you try.

See [doc/design.md](doc/design.md) for the rationale behind these APIs' shapes (why one template rather than four overloads, and why the constraint is `integral_like` rather than `std::signed_integral`).

### Type traits

`xstd::is_specialization_of` checks whether a type is a specialization of a class template with type parameters:

```cpp
#include <complex>
#include <xstd/type_traits.hpp>

static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
static_assert(not xstd::is_specialization_of_v<int, std::complex>);
```

`xstd::is_specialization_of` isn't fully general: its `Primary` parameter is constrained to `template<class...> class`, so it only accepts class templates whose parameters are all types. A template with a non-type parameter, like `std::array` (`template<class, size_t>`), doesn't just evaluate to `false` here - passing it as the second argument is a hard compile error, since its template template parameter kind doesn't match `template<class...> class`. See [doc/design.md](doc/design.md) for why.

`xstd::conditional_data_member_t` carries a data member only when a compile-time condition holds. C++ has no way to leave a member out, so the member always exists and its *type* is conditional: `Type` when the condition holds, `xstd::empty_type<Tag>` when it doesn't. Paired with `[[no_unique_address]]`, the absent case costs no storage:

```cpp
#include <xstd/type_traits.hpp>

template<bool HasBounds>
struct sprite
{
        [[no_unique_address]] xstd::conditional_data_member_t<HasBounds, unsigned, struct width_tag>  width;
        [[no_unique_address]] xstd::conditional_data_member_t<HasBounds, unsigned, struct height_tag> height;
        char frame;
};

static_assert(sizeof(sprite<false>) == sizeof(char));
static_assert(sizeof(sprite<true>) > sizeof(sprite<false>));
```

`Tag` names the member, not `Type`, and is required rather than deduced from it. Two `[[no_unique_address]]` subobjects of the same type must have distinct addresses, so absent members sharing one empty type stop overlapping: `sprite<false>` would be 2 bytes instead of 1. An elaborated-type-specifier declares each tag in place, so no separate declarations are needed.

`[[no_unique_address]]` is a no-op on MSVC's ABI, which spells it `[[msvc::no_unique_address]]`. The type-level behavior is the same everywhere; the layout saving is not.

### Concepts

`xstd::specialization_of` is the constraint spelling of `xstd::is_specialization_of`. It takes the type under test first, so partially applying it to a primary template gives a type-constraint:

```cpp
#include <complex>
#include <xstd/concepts.hpp>

template<xstd::specialization_of<std::complex> T>
constexpr auto real_part(T z) { return z.real(); }

static_assert(real_part(std::complex<double>(1.0, 2.0)) == 1.0);
```

It inherits the trait's `template<class...> class` restriction on `Primary`, including the hard error for a template with a non-type parameter.

`xstd::integral_like`, `xstd::signed_integral_like` and `xstd::unsigned_integral_like` are the open forms of `std::integral`, `std::signed_integral` and `std::unsigned_integral`. Those three are spelled over `std::is_integral`, which is a closed list the compiler owns; these are spelled over `xstd::is_integral_like_v`, which extends that list with `[iterator.concept.winc]`'s integer-class types — asking what a type does (`std::regular`, `std::three_way_comparable<T, std::strong_ordering>`, a `std::numeric_limits` specialization saying `is_integer`, explicit construction from `int`, and the full arithmetic, bitwise, shift, compound-assignment, and increment/decrement operator family) rather than what it is called, so a class type can satisfy them:

```cpp
#include <concepts>
#include <xstd/concepts.hpp>

static_assert(xstd::signed_integral_like<int>);
static_assert(std::signed_integral<int>);
static_assert(xstd::unsigned_integral_like<unsigned>);
static_assert(not xstd::signed_integral_like<double>);
```

These concepts include every `std::integral` type, including cv-qualified `bool`, and add structurally recognized integer-class types from [iterator.concept.winc]. `xstd::make_signed_like` and `xstd::make_unsigned_like` open the standard transformation traits to integer-class types. Built-in types are forwarded to the corresponding standard trait, while a class type is its own counterpart on the side it already occupies. The opposite direction is supplied with a one-line `std::type_identity` specialization. Unlike the standard transformations, the xstd primary templates are empty rather than ill-formed outside their domain, so their availability can be tested safely.

The naming follows one rule throughout: **an xstd entity that widens a standard one to class types keeps the standard name and appends `_like`.** The rule cuts both ways — a `_like` name exists only where there is a standard entity to widen — so what xstd adds is exactly the four the standard has:

| standard | xstd |
| :------- | :--- |
| `std::is_arithmetic` | `xstd::is_arithmetic_like` |
| `std::is_signed` | `xstd::is_signed_like` |
| `std::is_unsigned` | `xstd::is_unsigned_like` |
| `std::is_integral` | `xstd::is_integral_like` |

Each comes in both of the standard's spellings — the `bool_constant` named above, and the `_v` variable template that reads it — and each lives in `<xstd/type_traits.hpp>`. Which of them also gets a concept in `<xstd/concepts.hpp>` follows the same rule, applied to what `<concepts>` names rather than to what `<type_traits>` does:

| standard | trait | concept | xstd |
| :------- | :---- | :------ | :--- |
| arithmetic | `std::is_arithmetic` | — | `is_arithmetic_like` only |
| integral | `std::is_integral` | `std::integral` | both |
| signed integral | — | `std::signed_integral` | `signed_integral_like` only |

So there is deliberately no `is_signed_integral_like`, because `signed_integral` exists only as a concept — and no `arithmetic_like` concept, because `<concepts>` stops at `integral` / `signed_integral` / `unsigned_integral` / `floating_point` and never names `arithmetic`. A template that can do arithmetic on a type always knows which kind it needs; one that genuinely wants either writes `requires xstd::is_arithmetic_like_v<T>`, exactly as it would write `requires std::is_arithmetic_v<T>` today. See [doc/design.md](doc/design.md) for the second reason, which is about overload resolution rather than naming.

`<xstd/concepts/exposition_only.hpp>` is where the second half of `xstd::is_integral_like_v` is written out: `xstd::exposition_only::integer_class_type` is `[iterator.concept.winc]`'s integer-class type re-derived as a structural concept, since the standard's own version is a closed set of implementation-defined names no user type can join. It is installed because `<xstd/type_traits.hpp>` includes it, but it is exposition only in the standard's sense — it carries no stability promise, and everything it can answer is reachable as `xstd::is_integral_like_v` or `xstd::integral_like`.

```cpp
static_assert(xstd::is_signed_like_v<int> == std::is_signed_v<int>);   // agrees wherever std can answer
static_assert(std::conjunction_v<xstd::is_integral_like<int>, xstd::is_integral_like<long>>);
```

Opening `is_integral` is what opens the rest. It is the only one of the four with structural content — `[iterator.concept.winc]` says what a type has to *do* to be an integer, where "is this a number" is a marker rather than a behavior — so it is the one new judgement, and the other three are spelled over it the way the standard spells them over `std::is_integral_v`:

```cpp
is_arithmetic_like_v<T> == is_integral_like_v<T> or is_floating_point_v<T>   // the standard's own disjunction,
is_signed_like_v<T>     == is_arithmetic_like_v<T> and T(-1) < T(0)          // and its own two comparisons,
is_unsigned_like_v<T>   == is_arithmetic_like_v<T> and T(0) < T(-1)          // with the integral half opened
```

Only the integral half of `is_arithmetic_like_v` is open, because it is the only half xstd has a definition for: a class type whose `std::numeric_limits` says it is not an integer is deliberately *not* arithmetic-like today. Opening the floating-point half — for an extended-precision binary float, or a decimal type — is a second exposition-only concept and a second disjunct, with nothing else moving.

Each opened trait agrees with the one it widens on every cv-unqualified type that one can answer for — including answering rather than hard-erroring on arrays, `void` and incomplete types. See [doc/design.md](doc/design.md).

## Project layout

- `include/xstd/` contains the public header-only library code.
- `test/src/` contains the unit tests, with one executable generated per `.cpp` file.
- `doc/` contains historical proposal documents and [design.md](doc/design.md), which explains the rationale behind API and CI/toolchain choices; none of it is the current public API.

## Requirements

Using xstd requires a conforming [C++23](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) compiler and CMake 3.28 or later. Beyond those, nothing: the library is header-only, depends on no third-party code, and links against nothing, so adding it to a project adds no transitive requirements of its own. It is expected to work with any compiler that implements the C++23 features it uses. No compiler extension is needed and none is enabled — the headers compile in the strictly conforming dialect, and reach `__int128` there without one.

`std::format` for tuple-like types is the one library facility only *some* of xstd needs: it is required to format an `xstd::div_t`, and not to include the header or to use any other operation, since the `std::formatter` specialization's body is instantiated only where it is used.

Running xstd's own test suite does have dependencies, which consumers never build. They are listed in [CONTRIBUTING.md](CONTRIBUTING.md).

### Continuously tested toolchains

These header-only libraries are continuously being tested with the following conforming C++23 compilers, against all three mainstream standard libraries (libstdc++, the MSVC STL, and libc++). Following the model of [apt.llvm.org](https://apt.llvm.org/), which publishes its packages for a stable, a qualification and a development branch, we track the same three channels for every compiler: the established release, the newest release still being qualified, and the current development branch. Every leg in the table below is required, including every `Development` entry: a break there fails CI the same as a break on a stable release does.

| Platform | Compiler   | Standard Library | Stable                    | Qualification             | Development                    | CI    |
| :------- | :--------- | :--------------- | :------------------------ | :------------------------ | :----------------------------- | :---- |
| Linux    | GCC        | libstdc++        | 15                        | 16                        | 17-SVN                         | [![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml) |
| Windows  | MinGW      | libstdc++        | 15                        | 16                        | 17-SVN                         | [![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml) |
| Linux    | Clang      | libstdc++        | 22 (libstdc++ 15)         | 23 (libstdc++ 16)         | 24-SVN (libstdc++ 17-SVN)      | [![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml) |
| Linux    | Clang      | libc++           | 22                        | 23                        | 24-SVN                         | [![Clang-libc++](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml) |
| macOS    | Apple Clang | libc++          | 17.0.0 (Xcode 16.4)       | 21.0.0 (Xcode 26.6)       | —                              | [![Apple Clang](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml) |
| Windows  | Clang-CL   | MSVC             | 19.1.5 (VS 2022)          | 20.1.8 (VS 2026)          | 20.1.8 (VS 2026-Preview)       | [![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml) |
| Windows  | MSVC       | MSVC             | 2022 (17.11+)             | 2026                      | 2026-Preview                   | [![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml) |

See [doc/design.md](doc/design.md) for why some platforms have no `Development` entry, how each workflow provisions its development toolchain, and the MSVC version that first shipped tuple `std::formatter` support.

## License

Copyright Rein Halbersma 2014-2026.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
