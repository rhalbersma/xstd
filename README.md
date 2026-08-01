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

| Header                   | Additions          | Description | Reference |
| :-----                   | :--------          | :---------- | :-------- |
| `<xstd/array.hpp>`       | `array_from_types` | Create an `array` from a type list | none |
| `<xstd/cstdlib.hpp>`     | `sign` <br> `abs` <br> `uabs` <br> `div` <br> `div_t` <br> `euclidean_div` <br> `floored_div` | `constexpr`, any signed integral type <br> `constexpr`, any signed integral type <br> Total `\|x\|`, returning the unsigned type <br> `constexpr`, any signed integral type <br> `std::format` support, defaulted equality comparison <br> Euclidean division <br> Floored division | [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p2286r8](https://wg21.link/p2286r8) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/type_traits.hpp>` | `is_specialization_of` <br> `is_integral_constant` <br> `tagged_empty` <br> `optional_type` | Is a type a class template specialization? <br> Is a type an `integral_constant`? <br> A tagged empty type <br> An optional type | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>`     | `to_underlying` <br> `aligned_size` | `std::integral_constant` overload <br> Round a size up to a multiple of an alignment | none <br> none |

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

`xstd::aligned_size` rounds a requested size up to the next multiple of an alignment, e.g. to turn a bit count into a whole number of storage blocks. Its alignment must be nonzero and the rounded result must fit in `std::size_t`; violations are guarded by `assert`:

```cpp
#include <xstd/utility.hpp>

static_assert(xstd::aligned_size(100, 64) == 128);
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

// one template per operation, so every signed integral width is covered,
// including the two the labs/llabs/imaxabs naming has no name for
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

### Width and constraints

Every function in `<xstd/cstdlib.hpp>` is a single function template constrained to `std::signed_integral`, rather than the four fixed-width overloads (`abs`/`labs`/`llabs`/`imaxabs` and friends) `<cstdlib>` and `<cinttypes>` declare. Three consequences at the call site:

- **Every signed integral width is covered**, including `std::int8_t` and `std::int16_t`, which the `<cstdlib>` naming has no name for. 128-bit integers are the exception: `__int128` doesn't satisfy `std::integral` in the strictly conforming dialect this library targets, so it is not supported.
- **The result type is the argument type, not the promoted type.** `xstd::abs` of an `std::int16_t` is an `std::int16_t`, and its precondition is `std::int16_t`'s — the most negative value of the *argument* type is outside its contract (guarded by an `assert`), just like `MIN / -1` is for the division helpers. Callers who want the old promoting behavior write `xstd::abs(+x)` or `xstd::abs<int>(x)`.
- **Two-argument templates deduce one `T` from both arguments**, so a mixed-width call like `xstd::div(8, 3L)` is a deduction failure rather than a silent conversion. Spell the intent as `xstd::div<long>(8, 3L)`.

`xstd::sign` isn't part of `<cstdlib>`, and always returns a plain `int` whatever its argument's width.

See [doc/design.md](doc/design.md) for the rationale behind these APIs' shapes (why one template rather than four overloads, why 128-bit integers are out).

### Type traits

`xstd::is_specialization_of` checks whether a type is a specialization of a class template with type parameters:

```cpp
#include <complex>
#include <xstd/type_traits.hpp>

static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
static_assert(!xstd::is_specialization_of_v<int, std::complex>);
```

`xstd::is_specialization_of` isn't fully general: its `Primary` parameter is constrained to `template<class...> class`, so it only accepts class templates whose parameters are all types. A template with a non-type parameter, like `std::array` (`template<class, size_t>`), doesn't just evaluate to `false` here - passing it as the second argument is a hard compile error, since its template template parameter kind doesn't match `template<class...> class`. See [doc/design.md](doc/design.md) for why.

## Project layout

- `include/xstd/` contains the public header-only library code.
- `test/src/` contains the unit tests, with one executable generated per `.cpp` file.
- `doc/` contains historical proposal documents and [design.md](doc/design.md), which explains the rationale behind API and CI/toolchain choices; none of it is the current public API.

## Requirements

Using xstd requires a conforming [C++23](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) compiler and CMake 3.28 or later. Beyond those, nothing: the library is header-only, depends on no third-party code, and links against nothing, so adding it to a project adds no transitive requirements of its own. It is expected to work with any compiler that implements the C++23 features it uses, including `std::format` for tuple-like types.

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
