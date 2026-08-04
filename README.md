# Extensions to the C++ Standard Library

[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml)
[![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml)
[![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml)

xstd is a header-only C++23 library of small, portable standard-library
extensions. All public APIs are in namespace `xstd`.

## Requirements

- A conforming C++23 compiler
- CMake 3.28 or later when using the supplied CMake project
- No third-party runtime or library dependencies

`<xstd/format.hpp>` additionally needs standard-library support for formatting
tuple-like types. The other headers do not depend on `std::format`.

## Add xstd to a project

The usual approach is CMake's `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    xstd
    GIT_REPOSITORY https://github.com/rhalbersma/xstd.git
    GIT_TAG main # Prefer a release tag for reproducible builds.
)
FetchContent_MakeAvailable(xstd)

target_link_libraries(my_target PRIVATE xstd::xstd)
```

For an installed package, use `find_package(xstd CONFIG REQUIRED)`. For a
vendored checkout, use `add_subdirectory(external/xstd)`. Both provide the same
`xstd::xstd` target.

## Headers

| Header | Additions | Description | Reference |
| :----- | :-------- | :---------- | :-------- |
| `<xstd/concepts.hpp>` | `integral_like` <br> `signed_integral_like` <br> `unsigned_integral_like` <br> `specialization_of` | Constraint form of `is_integral_like` <br> Open form of `std::signed_integral` <br> Open form of `std::unsigned_integral` <br> Constraint form of `is_specialization_of` | [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) |
| `<xstd/config.hpp>` | `XSTD_NO_UNIQUE_ADDRESS` | Portable spelling of `[[no_unique_address]]`, including MSVC's ABI-compatible spelling | none |
| `<xstd/cstdint.hpp>` | `int128_t` <br> `uint128_t` | Platform 128-bit signed integer <br> Platform 128-bit unsigned integer | none <br> none |
| `<xstd/cstdlib.hpp>` | `div_t` <br> `sign` <br> `abs` <br> `unsigned_abs` <br> `div` <br> `euclidean_div` <br> `floored_div` | Defaulted equality comparison <br> `constexpr`, any signed integer-like type <br> `constexpr`, any signed integer-like type <br> Total `\|x\|`, returning the unsigned counterpart <br> `constexpr`, any signed integer-like type <br> Euclidean division <br> Floored division | none <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/format.hpp>` | `formatter<div_t>` | `std::format` support where the element type has it | [p2286r8](https://wg21.link/p2286r8), [p3391](https://wg21.link/p3391) (C++29, not yet implemented) |
| `<xstd/memory.hpp>` | `aligned_size` | Round a size up to a power-of-two alignment | none |
| `<xstd/ostream.hpp>` | `operator<<(ostream, div_t)` | Narrow stream support for test-framework diagnostics | none |
| `<xstd/type_traits.hpp>` | `empty_type` <br> `is_integral_like` <br> `is_arithmetic_like` <br> `is_signed_like` <br> `is_unsigned_like` <br> `is_specialization_of` <br> `make_signed_like` <br> `make_unsigned_like` <br> `conditional_data_member_t` | A tagged empty type <br> `std::is_integral`, opened to integer-class types <br> `std::is_arithmetic`, opened through the integral half <br> `std::is_signed`, opened the same way <br> `std::is_unsigned`, opened the same way <br> Is a type a class template specialization? <br> Open, user-specializable `std::make_signed` <br> Open, user-specializable `std::make_unsigned` <br> A conditionally present member | none <br> [iterator.concept.winc] (`is-integer-like`) <br> none <br> none <br> none <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>` | `to_underlying` | `std::integral_constant` overload | [p1682r1](https://wg21.link/p1682r1) (`std::to_underlying`) |

## Examples

```cpp
#include <climits>
#include <xstd/cstdlib.hpp>
#include <xstd/memory.hpp>

static_assert(xstd::unsigned_abs(INT_MIN) == static_cast<unsigned>(INT_MAX) + 1u);
static_assert(xstd::aligned_size(64, 100) == 128);

constexpr auto result = xstd::euclidean_div(-8, 3);
static_assert(result.quot == -3);
static_assert(result.rem == 1);
```

The numeric concepts and traits extend their standard counterparts to
integer-class types, including xstd's portable 128-bit aliases:

```cpp
#include <xstd/concepts.hpp>
#include <xstd/cstdint.hpp>

static_assert(xstd::signed_integral_like<xstd::int128_t>);
static_assert(xstd::unsigned_integral_like<xstd::uint128_t>);
```

Use `XSTD_NO_UNIQUE_ADDRESS` inside an attribute-specifier. It expands to
`msvc::no_unique_address` with the MSVC-compatible frontend and to
`no_unique_address` elsewhere:

```cpp
#include <xstd/config.hpp>

struct storage {
    [[XSTD_NO_UNIQUE_ADDRESS]] allocator_type allocator;
    value_type value;
};
```

`xstd::abs` has the same minimum-value precondition as signed built-in
absolute value. Following Rust's
[`unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs),
`xstd::unsigned_abs` returns the unsigned counterpart so that the full
magnitude is always representable.
The division functions require a nonzero divisor; `MIN / -1` is outside their
contract. Two-argument arithmetic functions require arguments of the same
type.

See [the design notes](doc/design.md) for API rationale and customization of
integer-class types. See [CONTRIBUTING.md](CONTRIBUTING.md) to build and test the
library itself.

## Continuously tested toolchains

CI covers the stable, qualification, and development channels of the major
C++23 compilers and standard libraries:

| Platform | Compiler   | Standard Library | Stable                    | Qualification             | Development                    | CI    |
| :------- | :--------- | :--------------- | :------------------------ | :------------------------ | :----------------------------- | :---- |
| Linux    | GCC        | libstdc++        | 15                        | 16                        | 17-SVN                         | [![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml) |
| Windows  | MinGW      | libstdc++        | 15                        | 16                        | 17-SVN                         | [![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml) |
| Linux    | Clang      | libstdc++        | 22 (libstdc++ 15)         | 23 (libstdc++ 16)         | 24-SVN (libstdc++ 17-SVN)      | [![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml) |
| Linux    | Clang      | libc++           | 22                        | 23                        | 24-SVN                         | [![Clang-libc++](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml) |
| macOS    | Apple Clang | libc++          | 17.0.0 (Xcode 16.4)       | 21.0.0 (Xcode 26.6)       | —                              | [![Apple Clang](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml) |
| Windows  | Clang-CL   | MSVC             | 19.1.5 (VS 2022)          | 20.1.8 (VS 2026)          | 20.1.8 (VS 2026-Preview)       | [![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml) |
| Windows  | MSVC       | MSVC             | 2022 (17.11+)             | 2026                      | 2026-Preview                   | [![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml) |

## License

Copyright Rein Halbersma 2014-2026.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
