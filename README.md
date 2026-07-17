# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml)
[![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml)
[![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml)
[![AppleClang](https://github.com/rhalbersma/xstd/actions/workflows/appleclang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/appleclang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd)

xstd is a header-only C++23 library for small standard-library extensions that can be implemented portably with stable compiler technology. It aims to prototype future-stdlib-style facilities without requiring experimental language features.

| Header                   | Additions          | Description | Reference |
| :-----                   | :--------          | :---------- | :-------- |
| `<xstd/array.hpp>`       | `array_from_types` | Create an `array` from a type list | none |
| `<xstd/cstdlib.hpp>`     | `sign`/`lsign`/`llsign`/`imaxsign` <br> `abs`/`labs`/`llabs`/`imaxabs` <br> `div`/`ldiv`/`lldiv`/`imaxdiv` <br> `div_t`/`ldiv_t`/`lldiv_t`/`imaxdiv_t` <br> `euclidean_div` and siblings <br> `floored_div` and siblings | `constexpr` support <br> `constexpr` support <br> `constexpr` support <br> `std::format` support, defaulted equality comparison <br> Euclidean division, at all 4 widths <br> Floored division, at all 4 widths | [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p2286r8](https://wg21.link/p2286r8) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/type_traits.hpp>` | `is_specialization_of` <br> `is_integral_constant` <br> `tagged_empty` <br> `optional_type` | Is a type a class template specialization? <br> Is a type an `integral_constant`? <br> A tagged empty type <br> An optional type | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>`     | `to_underlying`    | `std::integral_constant` overload | none |

## Using xstd

Link the header-only CMake target from your own target. When consuming an installed package:

```cmake
find_package(xstd 0.1 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE xstd::xstd)
```

When vendoring the repository as a subdirectory (xstd's own tests and their Boost.Test dependency are only built when xstd is the top-level project, so nothing needs to be disabled):

```cmake
add_subdirectory(external/xstd)
target_link_libraries(my_target PRIVATE xstd::xstd)
```

When using `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    xstd
    GIT_REPOSITORY https://github.com/rhalbersma/xstd.git
    GIT_TAG master # or a release tag
)
FetchContent_MakeAvailable(xstd)
target_link_libraries(my_target PRIVATE xstd::xstd)
```

The target publishes the public headers and requires C++23. You can also include individual headers directly, such as `<xstd/cstdlib.hpp>` or `<xstd/type_traits.hpp>`.

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

`xstd::div`, `xstd::euclidean_div`, and `xstd::floored_div` require a nonzero denominator. Like built-in signed integer division, `INT_MIN / -1` is outside their contract for `int` inputs. `xstd::div` follows C++'s truncated division semantics, `xstd::euclidean_div` always returns a nonnegative remainder, and `xstd::floored_div` returns a remainder with the divisor's sign unless the remainder is zero. Each has 3 wider siblings following `<cstdlib>`/`<cinttypes>`'s own naming: `ldiv`/`euclidean_ldiv`/`floored_ldiv` for `long`, `lldiv`/`euclidean_lldiv`/`floored_lldiv` for `long long`, and `imaxdiv`/`euclidean_imaxdiv`/`floored_imaxdiv` for `intmax_t`, returning `xstd::ldiv_t`, `xstd::lldiv_t`, and `xstd::imaxdiv_t` respectively.

Formatting any of these 4 `div_t`-like types requires C++23 standard-library support for formatting tuple-like values, because each formatter delegates to `std::formatter<std::tuple<T const&, T const&>>` through `std::tie`. This is covered by the continuously tested compiler and standard-library versions below. Each type also has a narrow `std::ostream& operator<<` overload (no wide-character support) that just forwards to `std::format`; it exists only so Boost.Test can print these types in test diagnostics, not as a general-purpose printing facility, so application code should prefer `std::format`/`std::print` directly.

`xstd::abs`, `xstd::labs`, `xstd::llabs`, and `xstd::imaxabs` are plain, non-template overloads mirroring `<cstdlib>`'s own `abs`/`labs`/`llabs` and `<cinttypes>`'s `imaxabs`: signed integral arguments only, no unsigned support. As with the built-in unary minus, the most negative value of each parameter type is outside its contract (guarded by an `assert`), just like `INT_MIN / -1` is for the division helpers. `xstd::sign`, `xstd::lsign`, `xstd::llsign`, and `xstd::imaxsign` aren't part of `<cstdlib>`, but follow the same plain, non-template style at the same 4 widths, each always returning a plain `int`. This differs from `boost::math::sign`, which is a single function template over any type; xstd deliberately narrows to 4 non-template, signed-integral-only overloads instead, for the same reasons as the `abs` family above.

`intmax_t` and `long` are the same type on some platforms (e.g. 64-bit Linux) and distinct types on others (e.g. Windows); the `imax`-prefixed names avoid the platform-dependent overload collision that naming them as `long`/`intmax_t` overloads of a single name would risk.

### Type traits

`xstd::is_specialization_of` checks whether a type is a specialization of a class template with type parameters:

```cpp
#include <complex>
#include <xstd/type_traits.hpp>

static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
static_assert(!xstd::is_specialization_of_v<int, std::complex>);
```

`xstd::is_specialization_of` isn't fully general: its `Primary` parameter is constrained to `template<class...> class`, so it only accepts class templates whose parameters are all types. A template with a non-type parameter, like `std::array` (`template<class, size_t>`), doesn't just evaluate to `false` here - passing it as the second argument is a hard compile error, since its template template parameter kind doesn't match `template<class...> class`. C++26 reflection may enable a more general version that also handles non-type template parameters.

## Building and testing

Configure, build, and run the test suite with CMake and CTest:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The repository also provides CMake presets for common local configurations:

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

Tests require Boost.Test. The checked-in vcpkg manifest declares that dependency for vcpkg-based builds, and the `*-vcpkg` presets pick up the toolchain from the `VCPKG_ROOT` environment variable:

```sh
cmake --preset dev-vcpkg
cmake --build --preset dev-vcpkg
ctest --preset dev-vcpkg
```

Alternatively, install Boost.Test with your system package manager and point CMake at the package if it is not found automatically.

## Project layout

- `include/xstd/` contains the public header-only library code.
- `test/src/` contains Boost.Test unit tests, with one executable generated per `.cpp` file.
- `doc/` contains historical proposal documents and design notes that provide background but are not the current public API.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for the development workflow and what a pull request must satisfy (full CI matrix, 100% line coverage, clean `clang-tidy`) before it can merge.

## Requirements

These header-only libraries are continuously being tested with the following conforming [C++23](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) compilers. Following the model of [apt.llvm.org](https://apt.llvm.org/), we support the latest two stable releases of each compiler, plus its current development / preview branch, on a best-effort basis:

| Platform | Compiler | Older stable | Latest stable | Trunk / Preview | Build |
| :------- | :------- | :------------ | :------------- | :---------------- | :---- |
| Linux    | GCC      | 15             | 16              | 17-SVN             | [![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml) |
| Linux    | Clang    | 21             | 22              | 23-SVN             | [![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml) |
| macOS    | AppleClang | 17.0.0 (Xcode 16.4) | 21.0.0 (Xcode 26.5) | —          | [![AppleClang](https://github.com/rhalbersma/xstd/actions/workflows/appleclang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/appleclang.yml) |
| Windows  | MinGW    | 15             | 16              | 17-SVN             | [![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml) |
| Windows  | Clang-CL | 19.1.5 (VS 2022) | 20.1.8 (VS 2026) | —               | [![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml) |
| Windows  | MSVC     | 2022 (17.11+)  | 2026            | 2026-Preview       | [![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml) |

The `Trunk / Preview` column is allowed to fail independently and does not affect the badges above. Neither `AppleClang` nor `Clang-CL` has a `Trunk / Preview` entry: Apple doesn't publish AppleClang dev snapshots the way LLVM does (the newest Xcode.app preinstalled on a runner image isn't reliably a distinct compiler), and "Clang tools for Windows" is a single VS component shared by the stable and preview MSVC toolsets alike, so it would just be the exact same `clang-cl.exe` as the `Latest stable` row. The `MinGW` workflow pins GCC versions through [WinLibs](https://winlibs.com) standalone builds rather than a rolling package feed, resolving the matching release from the GitHub API at run time; its `Trunk / Preview` leg tracks whatever snapshot build WinLibs currently publishes between stable branches, which does not always exist and is allowed to no-op. On Windows, formatting `xstd::div_t` sets the effective minimum toolset: the C++23 `formatter` specializations for `std::pair` and `std::tuple` ([P2286R8](https://wg21.link/p2286r8)) first shipped in the MSVC STL of Visual Studio 2022 17.11 (MSVC toolset 19.41), so any VS 2022 release from 17.11 onwards works. The CMake target requests C++23 through `target_compile_features(... cxx_std_23)`, letting CMake select the appropriate standard flag for each supported compiler.

All three mainstream standard libraries are exercised: libstdc++ (GCC, Clang, and MinGW legs), the MSVC STL (MSVC and Clang-CL legs), and libc++ (a best-effort `libc++` leg of the Clang workflow, which rebuilds Boost.Test against libc++ through the vcpkg overlay triplet in `.github/vcpkg`, plus the AppleClang legs, which use macOS's libc++ by default). The library is expected to work with any toolchain that implements the C++23 features above, including `std::format` for tuple-like types.

## License

Copyright Rein Halbersma 2014-2026.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
