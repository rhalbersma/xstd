# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml)
[![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml)
[![clang-cl](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml)

| Header                   | Additions          | Description | Reference |
| :-----                   | :--------          | :---------- | :-------- |
| `<xstd/array.hpp>`       | `array_from_types` | Create an `array` from a type list | none |
| `<xstd/cstdlib.hpp>`     | `abs` <br> `div` <br> `euclidean_div` <br> `floored_div` <br> `sign` | `constexpr std::abs(int)` <br> `constexpr std::div(int, int)` <br> Euclidean division <br> Floored division <br> `constexpr boost::math::sign` | [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) |
| `<xstd/type_traits.hpp>` | `is_specialization_of` <br> `is_integral_constant` <br> `tagged_empty` <br> `optional_type` | Is a type a class template specialization? <br> Is a type an `integral_constant`? <br> A tagged empty type <br> An optional type | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>`     | `to_underlying`    | Preserve a compile-time constant through `std::to_underlying` (C++23) | none |

## Requirements

These header-only libraries are continuously being tested with the following conforming [C++23](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/n4950.pdf) compilers. Following the model of [apt.llvm.org](https://apt.llvm.org/), we support the latest two stable releases of each compiler, plus its current development / preview branch, on a best-effort basis:

| Platform | Compiler | Older stable | Latest stable | Trunk / Preview | Build |
| :------- | :------- | :------------ | :------------- | :---------------- | :---- |
| Linux    | GCC      | 15             | 16              | 17-SVN             | [![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml) |
| Linux    | Clang    | 21             | 22              | 23-SVN             | [![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml) |
| Windows  | Clang (`clang-cl`) | —    | 20.1.8 (bundled) | —               | [![clang-cl](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml) |
| Windows  | MSVC     | 2022           | 2026            | 2026-Preview       | [![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml) |

The `Trunk / Preview` column is allowed to fail independently and does not affect the badges above. The `clang-cl` leg tests Clang's diagnostics against the MSVC STL, using whichever LLVM version the Windows runner image bundles. MSVC has no stable `/std:c++23` switch yet, so both MSVC rows build with `/std:c++latest`.

## License

Copyright Rein Halbersma 2014-2025.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
