#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# x64-linux, but built with the Clang under test and the libstdc++ it is
# paired with, instead of the runner image's default compiler. Used by the
# Clang workflow so that vcpkg's Boost.Test and the xstd tests share one
# standard library: the development leg compiles against the GCC trunk
# snapshot's libstdc++, and mixing that with the system one breaks
# Boost.Test at runtime.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../toolchains/clang.cmake)

# The ABI hash covers the compiler executable and the textual contents of
# this file and the chainloaded toolchain, but not the environment those
# read. The Clang version is implied by the executable; which libstdc++ it
# picks is not, so track it explicitly - otherwise binaries built against
# the trunk snapshot's libstdc++ would collide with binaries built by the
# same Clang against the runner's own.
set(VCPKG_ENV_PASSTHROUGH VCPKG_CLANG_FLAGS)
