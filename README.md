[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/) 
[![Standard](https://img.shields.io/badge/c%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0) 
[![](https://tokei.rs/b1/github/rhalbersma/xstd)](https://github.com/rhalbersma/xstd)

Extensions to the C++ Standard Library
--------------------------------------

| Header          | Additions          | Description | Reference |
| :-----          | :--------          | :---------- | :-------- |
| `<array>`       | `array_from_types` | Creates an `array` from a type list | none |
| `<cstddef>`     | `operator "" _uz`  | User-defined literals for `size_t` | [p0330r7](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0330r5.html) |
| `<cstdlib>`     | `abs` <br> `div` <br> `euclidean_div` <br> `floored_div` <br> `sign` | `constexpr std::abs(int)` <br> `constexpr std::div(int, int)` <br> Euclidean instead of truncated division <br> Floored instead of truncated division <br> `constexpr boost::math::sign` | [p0533r5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0533r5.pdf) <br> [p0533r5](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0533r5.pdf) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [Boost.Math](https://www.boost.org/doc/libs/1_67_0/libs/math/doc/html/math_toolkit/sign_functions.html) |
| `<type_traits>` | `any_of<T, Xs...>` <br> `tagged_empty<Tag>` <br> `or_empty<B, T>` | `(is_same_v<T, Xs> or ...)` <br> tagged empty base class <br> `conditional_t<B, T, tagged_empty<T>>` | none  <br> none <br> none |
| `<utility>`     |  `to_underlying`   | Converts an enum to its underlying type | [p1682r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1682r1.html) |

Requirements
------------

These header-only libraries are continuously being tested with the following conforming [C++17](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf) compilers:

| Platform | Compiler | Versions | Build |
| :------- | :------- | :------- | :---- |
| Linux    | Clang <br> GCC | 6.0, 7, 8, 9, 10-SVN<br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;7, 8, 9, 10-SVN | [![codecov](https://codecov.io/gh/rhalbersma/xstd/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd) <br> [![Build Status](https://travis-ci.org/rhalbersma/xstd.svg)](https://travis-ci.org/rhalbersma/xstd) |
| Windows  | Visual Studio  |                                         15.9.14 | [![Build status](https://ci.appveyor.com/api/projects/status/nu193iqabu749mpx?svg=true)](https://ci.appveyor.com/project/rhalbersma/xstd) |

License
-------

Copyright Rein Halbersma 2014-2019.  
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).  
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
