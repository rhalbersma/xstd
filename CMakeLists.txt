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
        include/xstd/concepts.hpp
        include/xstd/concepts/exposition_only.hpp
        include/xstd/concepts/integral_like.hpp
        include/xstd/concepts/nothrow_integral_operators.hpp
        include/xstd/concepts/signed_integral_like.hpp
        include/xstd/concepts/specialization_of.hpp
        include/xstd/concepts/unsigned_integral_like.hpp
        include/xstd/charconv.hpp
        include/xstd/charconv/to_chars.hpp
        include/xstd/cstdint.hpp
        include/xstd/cstdlib.hpp
        include/xstd/cstdlib/abs.hpp
        include/xstd/cstdlib/div.hpp
        include/xstd/cstdlib/div_t.hpp
        include/xstd/cstdlib/euclidean_div.hpp
        include/xstd/cstdlib/floored_div.hpp
        include/xstd/cstdlib/sign.hpp
        include/xstd/cstdlib/unsigned_abs.hpp
        include/xstd/format.hpp
        include/xstd/format/div_t.hpp
        include/xstd/memory.hpp
        include/xstd/type_traits.hpp
        include/xstd/type_traits/conditional_data_member.hpp
        include/xstd/type_traits/empty_type.hpp
        include/xstd/type_traits/is_arithmetic_like.hpp
        include/xstd/type_traits/is_integral_like.hpp
        include/xstd/type_traits/is_signed_like.hpp
        include/xstd/type_traits/is_specialization_of.hpp
        include/xstd/type_traits/is_unsigned_like.hpp
        include/xstd/type_traits/make_signed_like.hpp
        include/xstd/type_traits/make_unsigned_like.hpp
        include/xstd/utility.hpp
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
# Implementation configuration headers are installed because public headers
# include them, but are deliberately not part of the public HEADERS file set.
install(
    DIRECTORY include/xstd/config
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xstd
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
