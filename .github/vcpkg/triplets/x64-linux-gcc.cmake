#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# x64-linux, but built with the GCC under test instead of whichever one the
# runner image defaults to. Used by the GCC workflow so that vcpkg's
# Boost.Test and the xstd tests share one libstdc++: the trunk snapshot
# ships its own, newer one, and mixing the two breaks Boost.Test at runtime.
# vcpkg's ABI hash covers the compiler executable, so each GCC version gets
# its own universe of cached binaries without any further keying.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../toolchains/gcc.cmake)
