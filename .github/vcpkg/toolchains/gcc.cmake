#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Chainloaded by the x64-linux-gcc triplet: builds vcpkg ports with the same
# GCC the GCC workflow uses for xstd itself. The workflow supplies
# VCPKG_GCC_VERSION for each matrix leg.
set(CMAKE_C_COMPILER "gcc-$ENV{VCPKG_GCC_VERSION}")
set(CMAKE_CXX_COMPILER "g++-$ENV{VCPKG_GCC_VERSION}")
