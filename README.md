# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml)
[![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd)

xstd is a header-only C++23 library for small standard-library extensions that can be implemented portably with stable compiler technology. It aims to prototype future-stdlib-style facilities without requiring experimental language features.

| Header                   | Additions          | Description | Reference |
| :-----                   | :--------          | :---------- | :-------- |
| `<xstd/array.hpp>`       | `array_from_types` | Create an `array` from a type list | none |
| `<xstd/cstdlib.hpp>`     | `abs` <br> `labs` <br> `llabs` <br> `div` <br> `euclidean_div` <br> `floored_div` <br> `sign` | `constexpr std::abs(int)` <br> `constexpr std::labs(long)` <br> `constexpr std::llabs(long long)` <br> `constexpr std::div(int, int)` with tuple-style `std::format` support <br> Euclidean division <br> Floored division <br> `constexpr int sign(int)`, in the style of `boost::math::sign` | [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) |
| `<xstd/type_traits.hpp>` | `is_specialization_of` <br> `is_integral_constant` <br> `tagged_empty` <br> `optional_type` | Is a type a class template specialization? <br> Is a type an `integral_constant`? <br> A tagged empty type <br> An optional type | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>`     | `to_underlying`    | Preserve a compile-time constant through `std::to_underlying` (C++23) | none |

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

`xstd::div`, `xstd::euclidean_div`, and `xstd::floored_div` require a nonzero denominator. Like built-in signed integer division, `INT_MIN / -1` is outside their contract for `int` inputs. `xstd::div` follows C++'s truncated division semantics, `xstd::euclidean_div` always returns a nonnegative remainder, and `xstd::floored_div` returns a remainder with the divisor's sign unless the remainder is zero.

Formatting `xstd::div_t` requires C++23 standard-library support for formatting tuple-like values, because its formatter delegates to `std::formatter<std::tuple<int const&, int const&>>` through `std::tie`. This is covered by the continuously tested compiler and standard-library versions below.

`xstd::abs`, `xstd::labs`, and `xstd::llabs` are plain, non-template overloads mirroring `<cstdlib>`'s own `abs`/`labs`/`llabs` set: signed integral arguments only, no unsigned support. As with the built-in unary minus, the most negative value of each parameter type is outside its contract (guarded by an `assert`), just like `INT_MIN / -1` is for the division helpers. `xstd::sign` is not part of `<cstdlib>`, but follows the same plain, non-template style: it takes and returns a plain `int`.

### Type traits

`xstd::is_specialization_of` checks whether a type is a specialization of a class template with type parameters:

```cpp
#include <complex>
#include <xstd/type_traits.hpp>

static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
static_assert(!xstd::is_specialization_of_v<int, std::complex>);
```

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

When adding or changing a public utility:

1. Add or update the relevant header under `include/xstd/`.
2. Add or update matching tests under `test/src/`; CMake creates one test executable per `.cpp` file in that directory.
3. Run the CMake and CTest workflow above locally.
4. Update the feature table and examples in this README when the public API changes.

## Requirements

These header-only libraries are continuously being tested with the following conforming [C++23](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) compilers. Following the model of [apt.llvm.org](https://apt.llvm.org/), we support the latest two stable releases of each compiler, plus its current development / preview branch, on a best-effort basis:

| Platform | Compiler | Older stable | Latest stable | Trunk / Preview | Build |
| :------- | :------- | :------------ | :------------- | :---------------- | :---- |
| Linux    | GCC      | 15             | 16              | 17-SVN             | [![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml) |
| Linux    | Clang    | 21             | 22              | 23-SVN             | [![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml) |
| Windows  | Clang-CL | 19.1.5 (VS 2022, bundled) | 20.1.8 (VS 2026, bundled) | —               | [![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml) |
| Windows  | MSVC     | 2022 (17.11+)  | 2026            | 2026-Preview       | [![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml) |

The `Trunk / Preview` column is allowed to fail independently and does not affect the badges above. The `clang-cl` leg tests Clang's diagnostics against the MSVC STL, using whichever LLVM version each Visual Studio version bundles; there's no separate `Trunk / Preview` entry for it, since "Clang tools for Windows" is a single VS component shared by the stable and preview MSVC toolsets alike. On Windows, formatting `xstd::div_t` sets the effective minimum toolset: the C++23 `formatter` specializations for `std::pair` and `std::tuple` ([P2286R8](https://wg21.link/p2286r8)) first shipped in the MSVC STL of Visual Studio 2022 17.11 (MSVC toolset 19.41), so any VS 2022 release from 17.11 onwards works. The CMake target requests C++23 through `target_compile_features(... cxx_std_23)`, letting CMake select the appropriate standard flag for each supported compiler.

All three mainstream standard libraries are exercised: libstdc++ (GCC and Clang legs), the MSVC STL (MSVC and Clang-CL legs), and libc++ (a best-effort `libc++` leg of the Clang workflow, which rebuilds Boost.Test against libc++ through the vcpkg overlay triplet in `.github/vcpkg`). macOS / AppleClang is currently not tested; the library is expected to work with any toolchain that implements the C++23 features above, including `std::format` for tuple-like types.

## License

Copyright Rein Halbersma 2014-2026.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
