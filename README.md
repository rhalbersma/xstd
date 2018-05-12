Extensions to the C++ Standard Library
======================================

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/) 
[![Standard](https://img.shields.io/badge/c%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) 
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0) 
[![](https://tokei.rs/b1/github/rhalbersma/xstd)](https://github.com/rhalbersma/xstd)
[![Build Status](https://travis-ci.org/rhalbersma/xstd.svg?branch=master)](https://travis-ci.org/rhalbersma/xstd)
[![Build status](https://ci.appveyor.com/api/projects/status/nu193iqabu749mpx?svg=true)](https://ci.appveyor.com/project/rhalbersma/xstd)
[![codecov](https://codecov.io/gh/rhalbersma/xstd/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd)

**Additions to existing headers**

`<cstddef>`: User-Defined Literals for `size_t` (proposed as [N4254](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4254.html) and [P0330R0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0330r0.pdf)).

`<cstdlib>`: Floored and Euclidean versions of `std::div()` and equality operators on `std::div_t`, a `constexpr std::abs()` and `signum()`

`<type_traits>`: Conversions of scoped enums to their underlying types.

**New headers**

`<int_set>`: Rebooting the `std::bitset` franchise.

An `int_set<N, UIntType = std::size_t>` is a modern reimagining of `std::bitset<N>`, keeping what time has proven to be effective, and fixing or throwing out what is not. `int_set` does less (i.e. it does not do bounds-checking, and does not throw exceptions). `int_set` also has several iteration methods which makes it easier to use with the rest of the Standard Library.

Requirements
------------

These header-only libraries are usable with any compiler that implements the [C++17 Standard](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf). The supported compilers are [gcc](https://gcc.gnu.org/projects/cxx-status.html), version 7.3 or higher, [Clang](http://clang.llvm.org/cxx_status.html) version 6.0 or higher (tested both with libstdc++ 7.3 on Travis CI), and Visual C++ 15.6 or higher (tested on AppVeyor). Testing is done using CMake version 3.11 and Boost.Test version 1.66.0. See the Travis CI and AppVeyor config files for how to set this up on Linux and Windows.

License
-------

Copyright Rein Halbersma 2014 - 2018.   
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).   
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
