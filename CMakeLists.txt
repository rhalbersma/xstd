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

# Keep every target created by this project in the strictly conforming C++
# dialect. This is set before any targets are created so it also covers future
# non-test targets; consumers retain control over their own targets.
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
    ${PROJECT_NAME} INTERFACE
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set below adds its base
# directory to the build interface, and install(TARGETS ... FILE_SET) adds
# the install destination to the install interface.
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
        include/xstd/concepts/signed_integral_like.hpp
        include/xstd/concepts/specialization_of.hpp
        include/xstd/concepts/unsigned_integral_like.hpp
        include/xstd/cstdint.hpp
        include/xstd/cstdlib.hpp
        include/xstd/memory.hpp
        include/xstd/type_traits.hpp
        include/xstd/type_traits/conditional_data_member.hpp
        include/xstd/type_traits/integral_constant.hpp
        include/xstd/type_traits/is_arithmetic_like.hpp
        include/xstd/type_traits/is_integral_like.hpp
        include/xstd/type_traits/is_signed_like.hpp
        include/xstd/type_traits/is_specialization_of.hpp
        include/xstd/type_traits/is_unsigned_like.hpp
        include/xstd/type_traits/make_signed_like.hpp
        include/xstd/type_traits/make_unsigned_like.hpp
        include/xstd/utility.hpp
)

# Tests (and their Boost.Test dependency) only exist when xstd itself is
# being developed. Consumers vendoring this directory via add_subdirectory
# or FetchContent get just the header-only target, with no BUILD_TESTING
# workaround required on their side.
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

# Package config + version files, so consumers can locate the installed
# package with find_package(xstd CONFIG). ARCH_INDEPENDENT because a
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
