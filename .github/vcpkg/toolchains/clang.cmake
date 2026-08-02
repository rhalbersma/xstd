#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Chainloaded by the x64-linux-clang triplet: builds vcpkg ports with the
# same Clang and libstdc++ the Clang workflow uses for xstd itself. The
# workflow supplies VCPKG_CLANG_VERSION for each matrix leg, and
# VCPKG_CLANG_FLAGS for the leg whose libstdc++ lives outside /usr and has
# to be selected explicitly (--gcc-toolchain=).
set(CMAKE_C_COMPILER "clang-$ENV{VCPKG_CLANG_VERSION}")
set(CMAKE_CXX_COMPILER "clang++-$ENV{VCPKG_CLANG_VERSION}")
set(CMAKE_C_FLAGS_INIT "$ENV{VCPKG_CLANG_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "$ENV{VCPKG_CLANG_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "$ENV{VCPKG_CLANG_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "$ENV{VCPKG_CLANG_FLAGS}")
