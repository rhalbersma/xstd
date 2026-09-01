#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(
    xstd
    VERSION 0.1.0
    DESCRIPTION "Extensions to the C++ Standard Library"
    HOMEPAGE_URL "https://github.com/rhalbersma/xstd"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# The strictly conforming dialect, for every target this project creates.
# Set before any of them; consumers keep control over their own.
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
    ${PROJECT_NAME} INTERFACE
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set below adds its base
# directory, and install(TARGETS ... FILE_SET) adds the install destination.
target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_23
)

target_sources(
    ${PROJECT_NAME} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/xstd/ints.hpp
        include/xstd/ints/concepts.hpp
        include/xstd/ints/concepts/integer_class.hpp
        include/xstd/ints/concepts/integer.hpp
        include/xstd/ints/concepts/nothrow_const_operators.hpp
        include/xstd/ints/concepts/signed_integer.hpp
        include/xstd/core.hpp
        include/xstd/core/concepts.hpp
        include/xstd/core/concepts/specialization_of.hpp
        include/xstd/ints/concepts/unsigned_integer.hpp
        include/xstd/ints/charconv.hpp
        include/xstd/ints/charconv/to_chars.hpp
        include/xstd/ints/cstdint.hpp
        include/xstd/ints/cstdint/bit_int.hpp
        include/xstd/ints/cstdint/int128.hpp
        include/xstd/ints/cstdlib.hpp
        include/xstd/ints/cstdlib/abs.hpp
        include/xstd/ints/cstdlib/div.hpp
        include/xstd/ints/cstdlib/div_result.hpp
        include/xstd/ints/cstdlib/div_euclid.hpp
        include/xstd/ints/cstdlib/div_floor.hpp
        include/xstd/ints/cstdlib/sign.hpp
        include/xstd/ints/cstdlib/unsigned_abs.hpp
        include/xstd/ints/ext/absl.hpp
        include/xstd/ints/ext/absl/int128.hpp
        include/xstd/ints/ext/boost.hpp
        include/xstd/ints/ext/boost/int128.hpp
        include/xstd/ints/format.hpp
        include/xstd/ints/format/div_result.hpp
        include/xstd/ints/limits.hpp
        include/xstd/ints/limits/numeric_limits.hpp
        include/xstd/core/memory.hpp
        include/xstd/core/memory/align_up.hpp
        include/xstd/core/memory/align_down.hpp
        include/xstd/core/type_traits.hpp
        include/xstd/core/type_traits/conditional_data_member.hpp
        include/xstd/core/type_traits/empty_type.hpp
        include/xstd/ints/type_traits.hpp
        include/xstd/ints/type_traits/is_signed.hpp
        include/xstd/core/type_traits/is_specialization_of.hpp
        include/xstd/ints/type_traits/is_unsigned.hpp
        include/xstd/ints/type_traits/make_signed.hpp
        include/xstd/ints/type_traits/make_unsigned.hpp
        include/xstd/core/type_traits/no_unique_address.hpp
        include/xstd/ints/type_traits/promoted.hpp
        include/xstd/core/utility.hpp
        include/xstd/core/utility/to_underlying.hpp
)

# Tests and their Boost.Test dependency only exist when xstd is being
# developed; a consumer vendoring this directory gets the header-only target.
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
)
install(
    EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# So consumers can find_package(xstd CONFIG). ARCH_INDEPENDENT because a
# header-only library has no architecture-specific binaries.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
    ARCH_INDEPENDENT
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
